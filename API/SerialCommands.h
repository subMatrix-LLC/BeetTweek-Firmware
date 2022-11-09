#ifndef SERIALCOMMANDS_H
#define SERIALCOMMANDS_H

#define API_REV 1
#define INITIAL_CONNECTION_CODE "9915e814"




enum CommandTypeCode
{
    CommandTypeCode_Action = 0,
    CommandTypeCode_Request = 1,
	CommandTypeCode_NONE = 255
};


enum CommandAction
{
    CommandAction_ToggleStatusLed = 1,
	CommandAction_GoToBootMode,
	CommandAction_ResetSlaveController,
    CommandAction_SetRingLED,
    CommandAction_SetRingLEDRange,
    CommandAction_SetMotorModel,
    CommandAction_SetMotorModelParam,
	CommandAction_SetModuleSerialNumber,

	CommandAction_SetFrictionFactor,

	CommandAction_ToggleCalibrationMode,
	CommandAction_SetDAC0,
	CommandAction_SetDAC1,
	CommandAction_SetDAC2,
	CommandAction_SetDAC3,

	CommandAction_SetDACCAL0,
	CommandAction_SetDACCAL1,
	CommandAction_SetDACCAL2,
	CommandAction_SetDACCAL3,

	CommandAction_SetADCCAL0,
	CommandAction_SetADCCAL1,
	CommandAction_SetADCCAL2,
	CommandAction_SetADCCAL3,

	CommandAction_SaveUserCalibrationsToFactory,
	CommandAction_RestoreFactoryCalibrationsToUser,
    CommandAction_RestoreUserCalibrations,


	CommandAction_SetDrivePowerFactor,
	CommandAction_SetDriveOffset,
	CommandAction_SetMotorDirection,

	CommandAction_NONE = 255
};


enum CommandRequest
{
    CommandRequest_InitialConnectionCode = 1,
    CommandRequest_SerialAPIRev,
    CommandRequest_BoardModelString,
	CommandRequest_ModuleNameString,

    CommandRequest_MotorModelParam,

	CommandRequest_GetFrictionFactor,

	CommandRequest_GetDAC0,
	CommandRequest_GetDAC1,
	CommandRequest_GetDAC2,
	CommandRequest_GetDAC3,

	CommandRequest_GetADC0,
	CommandRequest_GetADC1,
	CommandRequest_GetADC2,
	CommandRequest_GetADC3,

	CommandRequest_GetADCRAW0,
	CommandRequest_GetADCRAW1,
	CommandRequest_GetADCRAW2,
	CommandRequest_GetADCRAW3,

	CommandRequest_GetADCVOLTS0,
	CommandRequest_GetADCVOLTS1,
	CommandRequest_GetADCVOLTS2,
	CommandRequest_GetADCVOLTS3,


	CommandRequest_GetADCCAL0,
	CommandRequest_GetADCCAL1,
	CommandRequest_GetADCCAL2,
	CommandRequest_GetADCCAL3,

	CommandRequest_GetDACCAL0,
	CommandRequest_GetDACCAL1,
	CommandRequest_GetDACCAL2,
	CommandRequest_GetDACCAL3,


	CommandRequest_FirmwareVersion,
    CommandRequest_ModuleSerialNumber,
	CommandRequest_IsHeadOfModule,
    CommandRequest_ManufactureDate,



	CommandRequest_GetDrivePowerFactor,
	CommandRequest_GetDriveOffset,
	CommandRequest_GetMotorDirection,

	CommandRequest_NONE = 255

};









#endif // SERIALCOMMANDS_H
