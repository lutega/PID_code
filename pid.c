
uint32_t l;
float out[20];
float buff_out[1000];
float buff_control[1000];
uint32_t control_iter = 0;
uint8_t temp[100];
uint8_t temp1[10];

/* Parameter PID dengan timesampling : 0.1 */
boolean flag_overrun = TRUE;
boolean flag_start = TRUE;

const float aaa = 0.0, bbb = 2.123;

const float kp = 0.224, ki = 2.242, kd = 0.0056;
const float Ts = 0.1;
volatile float propotional, integral, derivative;
float err = 0;
static volatile float err_prev = 0;

static volatile float control_signal = 0.0;
const float ref = 1.000;

/* Function to proses data yang dikirimkan dari Simulink untuk kebutuhan PIL */
boolean getSimulink(pchar args, void *data, IfxStdIf_DPipe *io)
{
    boolean flag = TRUE;
    l = g_shellInterface.cmd.length;
    // printf(args);
    uint32 i,j = 0;
    uint8 idx;

    if(args[0] == 'i'){
        switch(args[1]){
            // Handle input simulink;
            case '1':{
                if(flag_overrun){
                    /* Hitung sinyal kendali */
                    err = 1 - out[0];
                    propotional = kp*err;
                    integral    += ki*err*Ts;
                    derivative  = kd*((err - err_prev)*(1/Ts));
                    control_signal = propotional+integral+derivative;

                    /* limit control */
                    if(control_signal > 20) control_signal = 20;

                    err_prev = err;
                    buff_control[control_iter] = control_signal;
                    buff_out[control_iter] = out[0];
                    control_iter++;
                    IfxStdIf_DPipe_print(io, "%f",control_signal);
                    // printf("%f;%f\r\n", out[0], control_signal);
                    flag_overrun = FALSE;
                }
            }break;
            case '2':{
                IfxStdIf_DPipe_print(io, "%.3f",bbb);
            }break;
            case '3':{
                IfxStdIf_DPipe_print(io, "3");
            }break;
            default:{

            }break;
        }
    }

    if(args[0] == 'o'){
        // Handle output simulink;
        // Max output : 9
        // parsing data output simulink, dengan format : sim o%d;%.3f;%.3f#. %d -> jumlah data yg dikirim
        idx++;
        uint8_t i = 0,j = 0;

        for (idx = 3; idx < l; idx++){
            if(args[idx] != '#'){
                if(args[idx] != ';'){
                    temp[j+(10*i)] = (uint8_t) args[idx];
                    j++;
                }else{
                    j=0;
                    i++;
                }
            }
        }

        for (idx = 0; idx < args[1] - 0x30; idx++){
            memcpy(temp1,&temp[idx*10],10);
            out[idx] = atof(temp1);
            //out[idx] = atof(temp[idx*10]);
        }
        flag_overrun = TRUE;
    }
    return TRUE;
}