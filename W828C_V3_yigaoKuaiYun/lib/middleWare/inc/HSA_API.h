
/************************************************************************
 *
 * FileName      : HSA_API.h
 * ProjectName   :
 * Language      : C
 * Description   : The interface of the HSA library
 *
 ************************************************************************/

#ifndef _HSA_API_H_
#define _HSA_API_H_

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

//#include "hyhwAudioCfg.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

#define HSA_CALL_NO_ERR                                 (0)
#define HSA_CALL_ERR                                    (-1)
#define HSA_CALL_ERR_FIRMWARE_IMAGE_NO_SPACE            (-2)
#define HSA_CALL_ERR_FIRMWARE_IMAGE_NOT_FOUND           (-3)
#define HSA_CALL_ERR_FIRMWARE_IMAGE_NOT_VALID           (-4)
#define HSA_CALL_ERR_FIRMWARE_IMAGE_WRONG_CRC           (-5)
#define HSA_CALL_ERR_RESERVED_AREA_FILE_NOT_FOUND       (-6)
#define HSA_CALL_ERR_RESERVED_AREA_EMPTY                (-7)
#define HSA_CALL_ERR_RESERVED_AREA_NO_SPACE             (-8)
#define HSA_CALL_ERR_UPDATE_IMAGE_NOT_VALID             (-9)
#define HSA_CALL_ERR_UPDATE_IMAGE_NOT_FOUND             (-10)
#define HSA_CALL_ERR_UPDATE_IMAGE_WRONG_CRC             (-11)
#define HSA_CALL_ERR_UPDATE_IMAGE_NO_SPACE              (-12)
#define HSA_CALL_ERR_PERSISTENT_STORAGE_OUT_OF_RANGE    (-13)
#define HSA_CALL_ERR_FUNCTIONALITY_NOT_SUPPORTED		(-14)

#define HSA_NUM_SPECTRUMANALYSERBARS					(9)

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
//#define SHARK_HARDWARE_PLATFORM
//#define PLAY_USE_FILENAME

typedef enum
{
    HSA_PERIODIC_AIT,
    HSA_NOMINAL_AIT

} HSA_ApplicationInterfaceType_en;

typedef enum
{
    HSA_NORMAL_PM,
    HSA_FFW_ACTIVE_PM,
    HSA_FREV_ACTIVE_PM

} HSA_PlaybackModifier_en;

typedef enum
{
    HSA_PCM16_RAF,
    HSA_ADPCM_RAF,
	HSA_ADPCM_HISAMP_RAF,
	HSA_G729_RAF,
	HSA_MP3_RAF

} HSA_RecordAudioFormat_en;

typedef enum
{
    HSA_8_KHZ_SR,
    HSA_11_025_KHZ_SR,
    HSA_12_KHZ_SR,
    HSA_16_KHZ_SR,
    HSA_22_05_KHZ_SR,
    HSA_24_KHZ_SR,
    HSA_32_KHZ_SR,
    HSA_44_1_KHZ_SR

} HSA_SampleRate_en;

typedef enum
{
    HSA_FILEPATH_IIT,
    HSA_TOTALTIME_IIT,
    HSA_BITRATE_IIT,
    HSA_SAMPLE_RATE_IIT,
    HSA_FILETYPE_IIT,
    HSA_FILESIZE_IIT,
    HSA_ARTIST_IIT,
    HSA_SONGTITLE_IIT,
    HSA_ALBUM_IIT

} HSA_ItemInformationType_en;

typedef enum
{
    HSA_VOLUME_USED_SIZE_VSI,
    HSA_VOLUME_TOTAL_CAPACITY_VSI,
    HSA_VOLUME_NUMBER_OF_FILES_VSI,
    HSA_VOLUME_NUMBER_OF_TRACKS_VSI,
    HSA_VOLUME_PRESENT_VSI,
    HSA_VOLUME_FORMATTED_VSI    

} HSA_VolumeStatusItem_en;

typedef enum
{
    HSA_CURRENT_TITLE_ELAPSED_TIME_PIT,
    HSA_CURRENT_TITLE_TOTALTIME_PIT,
    HSA_CURRENT_TITLE_BITRATE_PIT,
    HSA_CURRENT_TITLE_SAMPLE_RATE_PIT,
    HSA_CURRENT_TITLE_FILETYPE_PIT,
    HSA_CURRENT_TITLE_ARTIST_PIT,
    HSA_CURRENT_TITLE_SONGTITLE_PIT,
    HSA_CURRENT_TITLE_ALBUM_PIT
    
} HSA_PlaybackInformationType_en;

typedef enum
{
    HSA_MP3_FT,
    HSA_WMA_FT,
    HSA_ASF_FT,
    HSA_AAC_FT,
    HSA_WAV_PCM_FT,
    HSA_WAV_ADPCM_FT,
    HSA_WAV_G729_FT,
    HSA_WAV_ADPCM_MTV_FT,
    HSA_WAV_FT,			/* 不确定的编码类型，可能是上面WAV开头的3种 */
    HSA_MID_FT,
    HSA_SWF_FT,
    HSA_MP4_FT,
    HSA_AVI_FT,
    HSA_MPG_FT,
   // HSA_FLV_FT,
    //HMV_FT,			/* hyctron 自定义格式，测试使用 */
    
    HSA_UNKNOWN_FT = 0xFF

} HSA_FileType_en;

typedef enum
{
    HSA_MILLISECOND_OT,
    HSA_SECTION_OT,
    HSA_BYTE_OT
} HSA_OffsetType_en;

typedef enum
{
    HSA_IDLE_AS,
    HSA_PLAYING_AS,
    HSA_PAUSED_AS,
    HSA_RECORDING_AS,
    HSA_RECORD_PAUSED_AS,
    HSA_HANDLING_USB_AS,
	HSA_TUNER_AS
} HSA_ApplicationState_en;

typedef enum
{
    HSA_TUNER_ATTENUATION_SP,
    HSA_ATTENUATION_SP,
    HSA_DOS_DATETIME_SP
} HSA_SystemParameter_en;

typedef enum
{
    HSA_USB_IDLE_UCS,
    HSA_USB_RECEIVING_UCS,
    HSA_USB_TRANSMITTING_UCS

} HSA_USBCommunicationStatus_en;

typedef enum
{
    HSA_DISABLE_ED,
    HSA_ENABLE_ED

} HSA_EnableDisable_en;

typedef enum
{
    HSA_MIC_NO_FEEDBACK_SS,
    HSA_LINE_NO_FEEDBACK_SS,
    HSA_MIC_FEEDBACK_SS,
    HSA_LINE_FEEDBACK_SS,
    HSA_PLAYBACK_SS,
    HSA_ANALOG_FEEDBACK_SS,
    HSA_NO_CODEC_SS

} HSA_SelectSource_en;

typedef enum
{
    HSA_READ_FOM,
    HSA_WRITE_FOM,
    HSA_RDWR_FOM

} HSA_FileOpenMode_en;

typedef enum
{
    HSA_OFFSET_FROM_START_FSS,
    HSA_OFFSET_FROM_CURRENT_FSS,
    HSA_OFFSET_FROM_END_FSS

} HSA_FileSeekSense_en;

typedef enum
{
    HSA_NO_EVENT_AF=-1,
    HSA_PLAYBACK_BUFFER_UNDERRUN_AF,
    HSA_END_OF_TRACK_AF,
    HSA_BEGINNING_OF_TRACK_AF,
    HSA_PLAYBACK_ERROR_AF,
    HSA_PLAYBACK_DRM_ERROR_AF,
    HSA_MEDIA_ACCESS_ERROR_AF,
    HSA_RECORD_FINISHED_AF,
    HSA_USB_CONNECTED_AF,
    HSA_USB_DISCONNECTED_AF,
    HSA_USB_SUSPENDED_AF,
    HSA_USB_UNSUSPEND_AF,
    HSA_VOLUME_REMOVED_AF

} HSA_ActiveFeedback_en;


typedef enum 
{
    HSA_SINGLE_DELETE_FDM,
    HSA_RECURSIVE_DIRECTORIES_FDM    

} HSA_FileDeleteMode_en;


#endif /* HSA_API_H_ */
