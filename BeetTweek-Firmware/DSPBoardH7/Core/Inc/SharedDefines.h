#ifndef SHAREDDEFINES_H
#define SHAREDDEFINES_H


#define maxVoltCalibrationPoint (9)
#define NUM_DAC_ADC_CAL_POINTS (maxVoltCalibrationPoint*2 + 1)
#define CAL_VOLT_HIGH (9.0f)
#define CAL_VOLT_LOW (-9.0f)

#define FRICTIONFACTOR_MIN (-2.0f)
#define FRICTIONFACTOR_MAX (2.0f)
#define FRICTIONFACTOR_DEF (0.496f)


#define DRIVEPOWERFACTOR_MIN (0.25f)
#define DRIVEPOWERFACTOR_MAX (1.0f)
#define DRIVEPOWERFACTOR_DEF (0.387f)

#define DRIVEOFFSET_MIN (-0.2f)
#define DRIVEOFFSET_MAX (0.2f)
#define DRIVEOFFSET_DEF (0.004f)
#endif
