#include "feature.hpp"
float calibrate() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Put at right:");

    bool validInput = false;
    char key_pressed = ' ';

    int N_steps_coarse = mm_to_steps(3,carrito);
    int N_steps_fine = mm_to_steps(1,carrito);

    int steps_to_left = 0;
    int steps_to_right = 0;

    while (validInput==false) {
        key_pressed = caminoBT.read();
        if (key_pressed == '1') {
            // go left
            carrito.setDirection(LOW);
            carrito.step(N_steps_coarse);
        } else if (key_pressed == '2') {
            // go right
            carrito.setDirection(HIGH);
            carrito.step(N_steps_coarse);
        } else if (key_pressed == '4') {
            // go left
            carrito.setDirection(LOW);
            carrito.step(N_steps_fine);
        } else if (key_pressed == '5') {
            // go right
            carrito.setDirection(HIGH);
            carrito.step(N_steps_fine);
        } else if (key_pressed == '9') {
            validInput = true;
        }
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Put at left:");
    validInput = false;
    key_pressed = ' ';
    // Each step is full step
    while ( validInput == false) {
        key_pressed = caminoBT.read();
        if (key_pressed == '1') {
            // go left
            carrito.setDirection(LOW);
            carrito.step(N_steps_coarse);
            steps_to_left+=N_steps_coarse;
        } else if (key_pressed == '2') {
            // go right
            carrito.setDirection(HIGH);
            carrito.step(N_steps_coarse);
            steps_to_right+=N_steps_coarse;
        } else if (key_pressed == '4') {
            // go left
            carrito.setDirection(LOW);
            carrito.step(N_steps_fine);
            steps_to_left+=N_steps_fine;
        } else if (key_pressed == '4') {
            // go right
            carrito.setDirection(HIGH);
            carrito.step(N_steps_fine);
            steps_to_right+=N_steps_fine;
        } else if (key_pressed == '#') {
            if ((steps_to_left-steps_to_right)>0) 
                validInput = true;
            else {
               validInput = true;
               lcd.setCursor(0,1);
                lcd.print("pbma : X0=0");
               steps_to_left=0;
               steps_to_right=0;
            } 
        }
    }    

    float X0_measured = steps_to_mm(steps_to_left-steps_to_right,carrito); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("X0 measured:");
    lcd.setCursor(0,1);
    lcd.print(X0_measured);
    delay(2000);
    return X0_measured; 
}

// Archimedean 
void barrido_archim(functions_t func) {
    float X0_measured = calibrate();
    vecint parametros;

    //ask_for_new_archimedean_params(); // TODO: make object oriented
    archimedean_param_t parameters;
    // TODO: add X_min
    // TODO: print everything in LCD of 4 lines
    EEPROM.get(ARCHIM_PARAM_ADDR, parameters);
    parameters.X0_measured = X0_measured;
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("UsingParam:"); 
    lcd.setCursor(0,1); 
    lcd.print(parameters.a);
    lcd.setCursor(7,1);     
    lcd.print(parameters.b);
    delay(2000);
    
    carrito.log = true; // DEBUG: esto es harcodeado para imprimir
    target.log = false; // DEBUG: esto es harcodeado
    
    // Calibracion (otro feature)
    // ask for new params: a, b, and X0
    target.setDirection(chosenDir);
    carrito.setDirection(chosenDir);
    
    wait_to_start(parametros);
    
    vecint2tparameterarch(parametros, &parameters);
    
    // Creacion de la tabla
    Table table = Table(); // TODO: make it a class method
    table_size = table.create(parameters, chosenMode);

    // Teclado para empezar

    //TODO: que diga si es arq o lineal
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   AutoMode");
    lcd.setCursor(0,1); 
    lcd.print("Press * to stop"); 

    initial_time = micros();
    carrito.init();
    target.init();
    
    // Loop
    char stopkey = '0'; // TODO: Pongo en cualquiera
    while (stopkey != '*' && must_stop == false) {
        carrito.move();
        target.move();
        stopkey = caminoBT.read();
    }
    Serial.println("Stopped.");
    
}

void vecint2tparameterarch(vecint v, archimedean_param_t* p){
    p->a=v[0];
    p->b=v[1];
    p->X0_measured=v[2]/2;
}

void vecint2tparameterlineal(vecint v, linear_param_t* p){
    p->linear_speed=v[0];
    p->angular_speed=v[1];
}

void barrido_linear(functions_t func) {
    vecint parametros;
    float X0_measured = calibrate();
    //ask_for_new_linear_params(); // TODO: make object oriented
    linear_param_t parameters;
    // TODO: add X_min
    EEPROM.get(LINEAR_PARAM_ADDR, parameters);
    parameters.X0_measured = X0_measured;

    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("UsingParam:"); 
    lcd.setCursor(0,1); 
    lcd.print(parameters.linear_speed);
    lcd.setCursor(7,1);     
    lcd.print(parameters.angular_speed);
    delay(2000);
    
    carrito.log = true; // DEBUG: esto es harcodeado para imprimir
    target.log = false; // DEBUG: esto es harcodeado
    
    // Calibracion (otro feature)

    target.setDirection(chosenDir);
    carrito.setDirection(chosenDir);
    
    wait_to_start(parametros);

    //verification que los parametros son los ingresado en la app
    /*Serial.println("linaer speed: "+ (String)parameters.linear_speed);
    Serial.println("angular speed: "+ (String)parameters.angular_speed);*/

    // Creacion de la tabla
    Table table = Table();
    table_size = table.create(parameters, chosenMode);
    
    //TODO: que diga si es arq o lineal
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   AutoMode");
    lcd.setCursor(0,1); 
    lcd.print("Press * to stop"); 

    initial_time = micros();
    carrito.init();
    target.init();
    
    // Loop
    char stopkey = '0'; // TODO: Pongo en cualquiera
    while (stopkey != '*' && must_stop == false) {
        carrito.move();
        target.move();
        stopkey = caminoBT.read();
    }
    Serial.println("Stopped.");
    
}

// Loop until button is hit
void wait_to_start(vecint parametros) {
    String sparametros;
    char button = '1';
    Serial.println("Press 0 to start:");

    lcd.print("Press 0 to start");
    int ind=0;
    while(button!='*') {
        button = caminoBT.read();
        if (button >='0' && button <=':'){
            sparametros=sparametros+button;
            ind++;
        }
        if (button == '.'){
            sparametros=sparametros+button;
            ind++;
        }
    }
    vchar2int(sparametros, ind, parametros);
    Serial.println("Starting..."); 
}

//"32.2:2:24:5"-> 32.2 2 24 5
void vchar2int(String vc, int tallevc, vecint v){
    int i=0;
    int separadorb=0;
    int separadorh=-1;
    String nb="";
    int index=0;
    while (i<=tallevc){
        nb="";
        separadorb=separadorh+1;
        i=separadorb;
        while  (vc[i]!=':' && i<tallevc){
            i++;
        }
        separadorh=i;
        for (int k=separadorb; k<separadorh; k++)
        {
            nb=nb+vc[k];
        }
        v[index]=nb.toFloat();
        index++;
    }
}


void manual_control(motor &chosenMotor) {
    int STEP_PARAM_ADDR = 0; // TODO: fix this
    //step_mode_t mode = chosenMotor.getStepMode();
    char key_pressed;
    bool want_to_exit = false;
    
    // Initial values
    //float distance_mm = 1; // [mm] TODO: llevar 
    //ask_for_new_step_param();
    step_param_t parameters;
    EEPROM.get(STEP_PARAM_ADDR, parameters);
    
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("UsingParam:"); 
    lcd.setCursor(0,1); 
    lcd.print(parameters.fine);
    lcd.setCursor(7,1);     
    lcd.print(parameters.coarse);
    delay(500);
    int N_steps_fine = mm_to_steps(parameters.fine, chosenMotor);
    int N_steps_coarse = mm_to_steps(parameters.coarse, chosenMotor);

    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print(" IZQ o DER");
    //lcd.setCursor(0,1);
    //lcd.print("3) o 6) DER");

    // Show in pantalla
    while(want_to_exit != true) {
        key_pressed = caminoBT.read();
        if (key_pressed == '4') {
            // go left
            chosenMotor.setDirection(LOW);
            chosenMotor.step(N_steps_coarse);
        } else if (key_pressed == '5') {
            // go right
            chosenMotor.setDirection(HIGH);
            chosenMotor.step(N_steps_coarse);
        } else if (key_pressed == '1') {
            // go left
            chosenMotor.setDirection(LOW);
            chosenMotor.step(N_steps_fine);
        } else if (key_pressed == '2') {
            // go right
            chosenMotor.setDirection(HIGH);
            chosenMotor.step(N_steps_fine);
        } else if (key_pressed == '9')
            want_to_exit = true;
    }
}


int get_distance(motor the_motor) {
    char char_dist;
    char_dist = customKeypad.waitForKey();

    // TODO: VALIDAAAR
    // TODO: hacer que pueda recibir dos digitos
    return mm_to_steps(char_dist - '0',the_motor);  
}

int mm_to_steps(float x_mm, motor the_motor) {
    return x_mm*the_motor.getStepsPerRevolution()/MM_PER_REV;
}

float steps_to_mm(int N_steps, motor the_motor) {
    Serial.println(N_steps);
    Serial.println(MM_PER_REV);
    Serial.println(the_motor.getStepsPerRevolution());
    return (N_steps*MM_PER_REV/the_motor.getStepsPerRevolution());
}
