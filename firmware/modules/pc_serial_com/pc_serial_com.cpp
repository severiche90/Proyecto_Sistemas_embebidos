//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "pc_serial_com.h"
#include "siren.h"
#include "fire_alarm.h"
#include "code.h"
#include "date_and_time.h"
#include "event_log.h"
#include "temp_hum_sensor.h"
#include "hum_suelo_sensor.h"

//=====[Declaration of private defines]======================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

Serial uartUsb(USBTX, USBRX);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );
static void pcSerialComCommandUpdate( char receivedChar );
static void availableCommands();
static void commandShowCurrentAlarmState();
static void commandShowCurrentOverTemperatureDetectorState();
static void commandShowCurrentOverHumidityDetectorState();
static void commandEnterCodeSequence();
static void commandEnterNewCode();
static void commandShowCurrentDht();
static void commandSetDateAndTime();
static void commandShowDateAndTime();
static void commandShowStoredEvents();
static void commandShowCurrentHumidity();

//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        receivedChar = uartUsb.getc();
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.printf( "%s", str );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    
    if( receivedChar != '\0' ) 
    {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;
            
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void pcSerialComGetCodeUpdate( char receivedChar )
{
    codeSequenceFromPcSerialCom[numberOfCodeChars] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeChars++;
   if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) 
   {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        codeComplete = true;
        numberOfCodeChars = 0;
   } 
}

static void pcSerialComSaveNewCodeUpdate( char receivedChar )
{
    static char newCodeSequence[CODE_NUMBER_OF_KEYS];

    newCodeSequence[numberOfCodeChars] = receivedChar;
    uartUsb.printf( "*" );
    numberOfCodeChars++;
    
    if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) 
    {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        numberOfCodeChars = 0;
        codeWrite( newCodeSequence );
        uartUsb.printf( "\r\nNew code configured\r\n\r\n" );
    } 
}

static void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) 
    {
        case '1': commandShowCurrentAlarmState(); break;
        case '2': commandShowCurrentOverTemperatureDetectorState(); break;
        case '3': commandShowCurrentOverHumidityDetectorState(); break;
        case '4': commandEnterCodeSequence(); break;
        case '5': commandEnterNewCode(); break;
        case 'a': case 'A': commandShowCurrentDht(); break; 
        case 'h': case 'H': commandShowCurrentHumidity(); break;
        case 's': case 'S': commandSetDateAndTime(); break;
        case 't': case 'T': commandShowDateAndTime(); break;
        case 'e': case 'E': commandShowStoredEvents(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    uartUsb.printf( "Comandos disponibles:\r\n" );
    uartUsb.printf( "Presione '1' Para obtener el estado de alarma\r\n" );
    uartUsb.printf( "Presione '2' Para obtener el estado del detector de sobretemperatura\r\n" );
    uartUsb.printf( "Presione '3' Para obtener el estado del detector de humedad del suelo\r\n" );
    uartUsb.printf( "Presione '4' Para ingresar el codigo para desactivar el alarma\r\n" );
    uartUsb.printf( "Presione '5' Para ingresar un nuevo codigo para desactivar el alarma\r\n" );
    uartUsb.printf( "Presione 'a' o 'A' Para obtener la temperatura y humedad del invernadero\r\n" );
    uartUsb.printf( "Presione 'h' o 'H' Para obtener la humedad del suelo\r\n" );
    uartUsb.printf( "Presione 's' o 'S' Para configurar la fecha y hora\r\n" );
    uartUsb.printf( "Presione 't' o 'T' Para obtener la fecha y hora\r\n" );
    uartUsb.printf( "Presione 'e' o 'E' Para obtener los eventos almacenados\r\n" );
    uartUsb.printf( "\r\n" );
}

static void commandShowCurrentAlarmState()
{
    if ( sirenStateRead() ) {
        uartUsb.printf( "La alarma esta activada\r\n");
    } else {
        uartUsb.printf( "La alarma no esta activada\r\n");
    }
}

static void commandShowCurrentOverTemperatureDetectorState()
{
    if ( overTemperatureDetectorStateRead() ) {
        uartUsb.printf( "Temperatura por encima del nivel maximo\r\n");
    } else {
        uartUsb.printf( "Temperatura por debajo del nivel maximo\r\n");
    }
}

static void commandShowCurrentOverHumidityDetectorState()
{
    if ( overHumidityDetectorStateRead() ) {
        uartUsb.printf( "Humedad del suelo no aceptable\r\n");
    } else {
        uartUsb.printf( "Humedad del suelo aceptable\r\n");
    }
}

static void commandEnterCodeSequence()
{
    if( sirenStateRead() ) 
    {
        uartUsb.printf( "Please enter the four digits numeric code " );
        uartUsb.printf( "to deactivate the alarm: " );
        pcSerialComMode = PC_SERIAL_GET_CODE;
        codeComplete = false;
        numberOfCodeChars = 0;
        
    } else {
        uartUsb.printf( "Alarm is not activated.\r\n" );
    }
}

static void commandEnterNewCode()
{
    uartUsb.printf( "Please enter the new four digits numeric code " );
    uartUsb.printf( "to deactivate the alarm: " );
    numberOfCodeChars = 0;
    pcSerialComMode = PC_SERIAL_SAVE_NEW_CODE;

}

static void commandShowCurrentDht()
{
    
    uartUsb.printf("Temperatura invernadero: %.1f °C\r\n", tempSensorReadCelsius());
    uartUsb.printf("Humedad invernadero: %.0f %%HR\r\n", humSensorRead());      
}

static void commandShowCurrentHumidity()
{
     
    uartUsb.printf("Humedad del suelo: %d %%\r\n", humedadSensorRead()); 
}

static void commandSetDateAndTime()
{
    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;
    
    uartUsb.printf("\r\nType de current year (YYYY) and press enter: ");
    uartUsb.scanf("%d", &year);
    uartUsb.printf("%d\r\n", year);

    uartUsb.printf("Type de current month (1-12) and press enter: ");
    uartUsb.scanf("%d", &month);
    uartUsb.printf("%d\r\n", month);

    uartUsb.printf("Type de current day (1-31) and press enter: ");
    uartUsb.scanf("%d", &day);
    uartUsb.printf("%d\r\n", day);

    uartUsb.printf("Type de current hour (0-23) and press enter: ");
    uartUsb.scanf("%d", &hour);
    uartUsb.printf("%d\r\n",hour);

    uartUsb.printf("Type de current minutes (0-59) and press enter: ");
    uartUsb.scanf("%d", &minute);
    uartUsb.printf("%d\r\n", minute);

    uartUsb.printf("Type de current seconds (0-59) and press enter: ");
    uartUsb.scanf("%d", &second);
    uartUsb.printf("%d\r\n", second);
    
    uartUsb.printf("Date and time has been set\r\n");

    while ( uartUsb.readable() ) 
    {
        uartUsb.getc();
    }

    dateAndTimeWrite( year, month, day, hour, minute, second );
}

static void commandShowDateAndTime()
{
    uartUsb.printf("Date and Time = %s\r\n", dateAndTimeRead());
}

static void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH];
    int i;
    
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) 
    {
        eventLogRead( i, str );
        uartUsb.printf( "%s\r\n", str );                       
    }
}