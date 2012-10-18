#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"

static float flight_loop_callback(float sinceCall, float sinceLoop, int counter, void *ref);

typedef struct {
    int serial_fd;
    unsigned char buffer[1024];
    unsigned int buffer_pos;
} state_t;

static state_t state = {0, {}, 0};
static XPLMDataRef data_refs[1];
static XPLMDataRef airspeed_data_ref;
static XPLMDataRef altitude_data_ref;
static XPLMDataRef heading_data_ref;
static XPLMDataRef brake_data_ref;
static XPLMDataRef oil_warn_data_ref;
static XPLMDataRef gen_warn_data_ref;
static XPLMDataRef throttle_data_ref;
static XPLMDataRef mixture_data_ref;

PLUGIN_API int XPluginStart(char *outName, char *outSignature, char *outDescription) {
    XPLMDebugString("[arduino] Plugin started\n");
    strcpy(outName, "Serial-attached cockpit");
    strcpy(outSignature, "danellis.serial_cockpit");
    strcpy(outDescription, "Use and control cockpit hardware attached to a serial port");

    XPLMDebugString("[arduino] Registering flight loop callback\n");
    XPLMRegisterFlightLoopCallback(flight_loop_callback, -1.0, 0);

    return 1;
}

PLUGIN_API void XPluginStop(void) {
    XPLMDebugString("[arduino] Plugin stopped\n");
    XPLMDebugString("[arduino] Unregistering flight loop callback\n");
    XPLMUnregisterFlightLoopCallback(flight_loop_callback, 0);
}

PLUGIN_API void XPluginEnable(void) {
    data_refs[0] = XPLMFindDataRef("sim/cockpit/electrical/battery_on");
    airspeed_data_ref = XPLMFindDataRef("sim/cockpit2/gauges/indicators/airspeed_kts_pilot");
    altitude_data_ref = XPLMFindDataRef("sim/cockpit2/gauges/indicators/altitude_ft_pilot");
    heading_data_ref = XPLMFindDataRef("sim/flightmodel/position/magpsi");
    brake_data_ref = XPLMFindDataRef("sim/flightmodel/controls/parkbrake");
    oil_warn_data_ref = XPLMFindDataRef("sim/cockpit/warnings/annunciators/oil_pressure_low");
    gen_warn_data_ref = XPLMFindDataRef("sim/cockpit/warnings/annunciators/generator");
    throttle_data_ref = XPLMFindDataRef("sim/cockpit2/engine/actuators/throttle_ratio_all");
    mixture_data_ref = XPLMFindDataRef("sim/cockpit2/engine/actuators/mixture_ratio_all");

    XPLMDebugString("[arduino] Plugin enabled\n");
    if ((state.serial_fd = open("/dev/tty.usbserial-A8007qt7", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1) {
        XPLMDebugString("[arduino] Opening serial device failed\n");
        return;
    }
    XPLMDebugString("[arduino] Opened serial device\n");

    struct termios options;
    tcgetattr(state.serial_fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(state.serial_fd, TCSANOW, &options);
    XPLMDebugString("[arduino] Serial device configured\n");
}

PLUGIN_API void XPluginDisable(void) {
    XPLMDebugString("[arduino] Plugin disabled\n");
    close(state.serial_fd);
    XPLMDebugString("[arduino] Closed serial device\n");
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMessage, void *inParam) {

}

static float flight_loop_callback(float sinceCall, float sinceLoop, int counter, void *ref) {
    unsigned char output[2];

    float airspeed = XPLMGetDataf(airspeed_data_ref);
    // float altitude = XPLMGetDataf(altitude_data_ref);
    // float heading = XPLMGetDataf(heading_data_ref);
    float brake = XPLMGetDataf(brake_data_ref);
    int oil_warn;
    XPLMGetDatavi(oil_warn_data_ref, &oil_warn, 0, 1);
    int gen_warn = XPLMGetDatai(gen_warn_data_ref);

    output[0] = 0;
    output[1] = ((int) airspeed / 10) % 10;
    write(state.serial_fd, &output, 2);

    /* output[0] = 1;
    output[1] = 180 - heading;
    write(state.serial_fd, &output, 2); */

    output[0] = 2;
    output[1] = brake * 255;
    write(state.serial_fd, &output, 2);

    output[0] = 3;
    output[1] = oil_warn;
    write(state.serial_fd, &output, 2);

    output[0] = 4;
    output[1] = gen_warn;
    write(state.serial_fd, &output, 2);

    for (;;) {
        int read_result = read(state.serial_fd, &state.buffer[state.buffer_pos], 1);
        switch (read_result) {
            case -1:
                if (errno == EAGAIN) {
                    return -1.0;
                }
                XPLMDebugString("[arduino] read failed -- stopping flight loop\n");
                return 0.0;

            case 0:
                return -1.0;

            case 1:
                state.buffer_pos += 1;
                switch (state.buffer[0]) {
                    case 0:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'switch off' command\n");
                            XPLMSetDatai(data_refs[0], 0);
                            state.buffer_pos = 0;
                        }
                        break;

                    case 1:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'switch on' command\n");
                            XPLMSetDatai(data_refs[0], 1);
                            state.buffer_pos = 0;
                        }
                        break;

                    case 2:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'button press' command\n");
                            XPLMCommandButtonPress(xplm_joy_start_all);
                            state.buffer_pos = 0;
                        }
                        break;

                    case 3:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'button release' command\n");
                            XPLMCommandButtonRelease(xplm_joy_start_all);
                            state.buffer_pos = 0;
                        }
                        break;

                    case 4:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'throttle' value\n");
                            XPLMSetDataf(throttle_data_ref, state.buffer[1] / 255.0);
                            state.buffer_pos = 0;
                        }

                    case 5:
                        if (state.buffer_pos == 2) {
                            XPLMDebugString("[arduino] Received 'mixture' value\n");
                            XPLMSetDataf(mixture_data_ref, state.buffer[1] / 255.0);
                            state.buffer_pos = 0;
                        }
                }
                break;
        }
    }
}
