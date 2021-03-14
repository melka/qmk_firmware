EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x20 INTEL_BOTTOM1
U 1 1 6032ADE6
P 1950 2450
F 0 "INTEL_BOTTOM1" H 2030 2442 50  0000 L CNN
F 1 "Conn_01x20" H 2030 2351 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 1950 2450 50  0001 C CNN
F 3 "~" H 1950 2450 50  0001 C CNN
	1    1950 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 1550 1650 1550
Wire Wire Line
	1750 1650 1650 1650
Wire Wire Line
	1750 1750 1650 1750
Wire Wire Line
	1750 1850 1650 1850
Wire Wire Line
	1750 1950 1650 1950
Wire Wire Line
	1750 2050 1650 2050
Wire Wire Line
	1750 2150 1650 2150
Wire Wire Line
	1750 2250 1650 2250
Wire Wire Line
	1750 2350 1650 2350
Wire Wire Line
	1750 2450 1650 2450
Wire Wire Line
	1750 2550 1650 2550
Wire Wire Line
	1750 2650 1650 2650
Wire Wire Line
	1750 2750 1650 2750
Wire Wire Line
	1750 2850 1650 2850
Wire Wire Line
	1750 2950 1650 2950
Wire Wire Line
	1750 3050 1650 3050
Wire Wire Line
	1750 3150 1650 3150
Wire Wire Line
	1750 3250 1650 3250
Wire Wire Line
	1750 3350 1650 3350
$Comp
L Connector_Generic:Conn_01x20 INTEL_TOP1
U 1 1 6033AC00
P 3050 2450
F 0 "INTEL_TOP1" H 2968 3567 50  0000 C CNN
F 1 "Conn_01x20" H 2968 3476 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 3050 2450 50  0001 C CNN
F 3 "~" H 3050 2450 50  0001 C CNN
	1    3050 2450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3250 1650 3350 1650
Wire Wire Line
	3250 1750 3350 1750
Wire Wire Line
	3250 1850 3350 1850
Wire Wire Line
	3250 1950 3350 1950
Wire Wire Line
	3250 2050 3350 2050
Wire Wire Line
	3250 2150 3350 2150
Wire Wire Line
	3250 2250 3350 2250
Wire Wire Line
	3250 2350 3350 2350
Wire Wire Line
	3250 2450 3350 2450
Wire Wire Line
	3250 2550 3350 2550
Wire Wire Line
	3250 2650 3350 2650
Wire Wire Line
	3250 2750 3350 2750
Wire Wire Line
	3250 2850 3350 2850
Wire Wire Line
	3250 2950 3350 2950
Wire Wire Line
	3250 3050 3350 3050
Wire Wire Line
	3250 3150 3350 3150
Wire Wire Line
	3250 3250 3350 3250
Wire Wire Line
	3250 3350 3350 3350
Wire Wire Line
	3250 3450 3350 3450
Text GLabel 3350 3450 2    50   Input ~ 0
MUX_A
Text GLabel 3350 3350 2    50   Input ~ 0
MUX_B
Text GLabel 3350 3250 2    50   Input ~ 0
MUX_C
Text GLabel 3350 3150 2    50   Input ~ 0
MUX_D
Text GLabel 3350 2950 2    50   Input ~ 0
5V
Text GLabel 3350 2850 2    50   Input ~ 0
ROW_0
Text GLabel 3350 2750 2    50   Input ~ 0
ROW_1
Text GLabel 3350 2650 2    50   Input ~ 0
ROW_2
Text GLabel 3350 2550 2    50   Input ~ 0
ROW_3
Text GLabel 3350 2450 2    50   Input ~ 0
ROW_4
Text GLabel 3350 2350 2    50   Input ~ 0
ROW_5
Text GLabel 3350 2250 2    50   Input ~ 0
ROW_6
Text GLabel 3350 2150 2    50   Input ~ 0
ROW_7
Text GLabel 1650 3350 0    50   Input ~ 0
LED_CAPS
Text GLabel 1650 3250 0    50   Input ~ 0
LED_NUM
Text GLabel 1650 3150 0    50   Input ~ 0
ENC_LED_V
Text GLabel 1650 3050 0    50   Input ~ 0
ENC_LED_H
Text GLabel 1650 2850 0    50   Input ~ 0
ENC_HALL_B
Text GLabel 1650 2950 0    50   Input ~ 0
ENC_HALL_A
Text GLabel 1650 2450 0    50   Input ~ 0
LED_CLK
$Comp
L power:GND #PWR01
U 1 1 6034D2EF
P 1650 3450
F 0 "#PWR01" H 1650 3200 50  0001 C CNN
F 1 "GND" V 1655 3322 50  0000 R CNN
F 2 "" H 1650 3450 50  0001 C CNN
F 3 "" H 1650 3450 50  0001 C CNN
	1    1650 3450
	0    1    1    0   
$EndComp
Wire Wire Line
	1750 3450 1650 3450
$Comp
L power:+5V #PWR05
U 1 1 6038323D
P 3350 1550
F 0 "#PWR05" H 3350 1400 50  0001 C CNN
F 1 "+5V" V 3365 1678 50  0000 L CNN
F 2 "" H 3350 1550 50  0001 C CNN
F 3 "" H 3350 1550 50  0001 C CNN
	1    3350 1550
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 1550 3250 1550
Wire Wire Line
	4650 1550 4550 1550
Wire Wire Line
	4650 1650 4550 1650
Wire Wire Line
	4650 1750 4550 1750
Wire Wire Line
	4650 1850 4550 1850
Wire Wire Line
	4650 1950 4550 1950
Wire Wire Line
	4650 2050 4550 2050
Wire Wire Line
	4650 2150 4550 2150
Wire Wire Line
	4650 2250 4550 2250
Wire Wire Line
	4650 2350 4550 2350
Wire Wire Line
	4650 2450 4550 2450
Wire Wire Line
	4650 2550 4550 2550
Wire Wire Line
	4650 2650 4550 2650
Wire Wire Line
	4650 2750 4550 2750
Wire Wire Line
	4650 2850 4550 2850
Wire Wire Line
	4650 2950 4550 2950
Wire Wire Line
	4650 3050 4550 3050
Wire Wire Line
	4650 3150 4550 3150
Wire Wire Line
	4650 3250 4550 3250
Wire Wire Line
	4650 3350 4550 3350
Wire Wire Line
	4650 3450 4550 3450
Wire Wire Line
	5450 1850 5550 1850
Wire Wire Line
	5450 1950 5550 1950
Wire Wire Line
	5450 2050 5550 2050
Wire Wire Line
	5450 2150 5550 2150
Wire Wire Line
	5450 2250 5550 2250
Wire Wire Line
	5450 2350 5550 2350
Wire Wire Line
	5450 2450 5550 2450
Wire Wire Line
	5450 2550 5550 2550
Wire Wire Line
	5450 2650 5550 2650
Wire Wire Line
	5450 2750 5550 2750
Wire Wire Line
	5450 2850 5550 2850
Wire Wire Line
	5450 2950 5550 2950
Wire Wire Line
	5450 3150 5550 3150
Wire Wire Line
	5450 3250 5550 3250
Wire Wire Line
	5450 3350 5550 3350
Wire Wire Line
	5450 3450 5550 3450
$Comp
L power:GND #PWR010
U 1 1 603986F2
P 4550 3350
F 0 "#PWR010" H 4550 3100 50  0001 C CNN
F 1 "GND" V 4555 3222 50  0000 R CNN
F 2 "" H 4550 3350 50  0001 C CNN
F 3 "" H 4550 3350 50  0001 C CNN
	1    4550 3350
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR09
U 1 1 60398704
P 4550 3250
F 0 "#PWR09" H 4550 3100 50  0001 C CNN
F 1 "+5V" V 4565 3378 50  0000 L CNN
F 2 "" H 4550 3250 50  0001 C CNN
F 3 "" H 4550 3250 50  0001 C CNN
	1    4550 3250
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR011
U 1 1 6039870A
P 4550 3450
F 0 "#PWR011" H 4550 3300 50  0001 C CNN
F 1 "+3.3V" V 4565 3578 50  0000 L CNN
F 2 "" H 4550 3450 50  0001 C CNN
F 3 "" H 4550 3450 50  0001 C CNN
	1    4550 3450
	0    -1   -1   0   
$EndComp
Text GLabel 4550 3150 0    50   Input ~ 0
LED_CLK
Text GLabel 4550 3050 0    50   Input ~ 0
ENC_HALL_A
Text GLabel 4550 2950 0    50   Input ~ 0
ENC_HALL_B
NoConn ~ 4550 1950
NoConn ~ 4550 2050
NoConn ~ 4550 2150
NoConn ~ 4550 2250
NoConn ~ 4550 2350
NoConn ~ 5550 1950
NoConn ~ 5550 3450
NoConn ~ 5550 3350
Text GLabel 4550 2850 0    50   Input ~ 0
MUX_D
Text GLabel 4550 2750 0    50   Input ~ 0
MUX_C
Text GLabel 4550 2650 0    50   Input ~ 0
MUX_B
Text GLabel 4550 2550 0    50   Input ~ 0
MUX_A
Text GLabel 4550 1750 0    50   Input ~ 0
LED_NUM
Text GLabel 4550 1850 0    50   Input ~ 0
LED_CAPS
Text GLabel 4550 1550 0    50   Input ~ 0
ENC_LED_H
Text GLabel 4550 1650 0    50   Input ~ 0
ENC_LED_V
Text GLabel 5550 2950 2    50   Input ~ 0
ROW_0
Text GLabel 5550 2850 2    50   Input ~ 0
ROW_1
Text GLabel 5550 2750 2    50   Input ~ 0
ROW_2
Text GLabel 5550 2650 2    50   Input ~ 0
ROW_3
Text GLabel 5550 2550 2    50   Input ~ 0
ROW_4
Text GLabel 5550 2450 2    50   Input ~ 0
ROW_5
Text GLabel 5550 2350 2    50   Input ~ 0
ROW_6
Text GLabel 5550 2250 2    50   Input ~ 0
ROW_7
NoConn ~ 3350 3050
NoConn ~ 3350 1750
NoConn ~ 3350 1850
NoConn ~ 3350 1950
NoConn ~ 3350 2050
NoConn ~ 3350 1650
NoConn ~ 1650 1550
NoConn ~ 1650 1650
NoConn ~ 1650 1750
NoConn ~ 1650 1850
NoConn ~ 1650 1950
NoConn ~ 1650 2050
NoConn ~ 1650 2150
NoConn ~ 1650 2250
NoConn ~ 1650 2350
NoConn ~ 1650 2550
NoConn ~ 1650 2650
NoConn ~ 1650 2750
$Comp
L pcaz1:blackpill U1
U 1 1 6033390F
P 5050 2150
F 0 "U1" H 5050 2965 50  0000 C CNN
F 1 "blackpill" H 5050 2874 50  0000 C CNN
F 2 "PCA-Z1:blackpill" H 5050 2150 50  0001 C CNN
F 3 "" H 5050 2150 50  0001 C CNN
	1    5050 2150
	1    0    0    -1  
$EndComp
NoConn ~ 4550 2450
NoConn ~ 5550 1850
NoConn ~ 5550 2050
NoConn ~ 5550 2150
NoConn ~ 5950 8500
$EndSCHEMATC
