/**
 * @mainpage Control System Documentation
 *
 * @section intro_sec Introduction
 * The system includes functionality for:
 * - Parameter setting (SET)
 * - Parameter reading (GET) 
 * - Status monitoring and alarm generation (ALARM)
 *
 * @section features_sec Main Features
 *
 * - Output power control (0-10 dBm)
 * - Frequency setting (25.0-26.0 MHz with 0.1 MHz step)
 * - Automatic and manual modulation
 * - Temperature, input and output power monitoring
 * - Alarm system with different criticality levels
 *
 * @section arch_sec Architecture
 *
 * The system consists of the following main components:
 *
 * @subsection data_sec SystemData
 * Central class for storing all system parameters and status data.
 *
 * @subsection system_sec System  
 * Base class providing common functionality for all components.
 *
 * @subsection set_sec SET
 * Class for setting system parameters with input data validation.
 *
 * @subsection get_sec GET
 * Class for reading current system parameters.
 *
 * @subsection alarm_sec ALARM
 * Class for system status monitoring and alarm generation.
 *
 * @section alarm_levels_sec Alarm Levels
 *
 * - Warning: Warnings about approaching critical values
 * - Error: Errors when exceeding safe ranges
 * - Critical: Critical errors requiring immediate intervention
 */