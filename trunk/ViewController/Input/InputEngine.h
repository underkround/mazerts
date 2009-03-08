/**
 * ID3DApplication.h header file
 * Interface for the CImputEngine class
 * @author d4472
 * @date 26.1.2009
 *
 * This class is a single wrapper for direct input
 * Class is cabable of retrieving information
 * attached input devices
 *
 * REQUIRES:
 * Project wide define: DIRECTINPUT_VERSION=0x900
 * Libraries: dinput8.lib
 */
 
#pragma once
 
#include <tchar.h>
#include <dinput.h>
 
// include STL vector to store found input
// devices in dynamic array
#include <vector>
using namespace std;
 
class CInputEngine
{
public:
    struct DEVICE
    {
        GUID            gdDevice; //in windows every device has it's own guid
        TCHAR            strName[256]; //and name
    };
 
    CInputEngine(void);
    virtual ~CInputEngine(void);
 
    /**
     * Create
     * initialize the input engine
     * @param hWnd handle to application main window
     * @return S_OK or an error code
     */
    HRESULT Create(HWND hWnd);
 
    /**
     * Release
     * release input engine
     */
    void Release(void);
 
    /**
     * GetKeyboards
     * get all attached keyboard devices
     * @param arrDevices reference to array of devices, filled by function
     * @return number of devices added to 'arrDevices'
     */
    int GetKeyboards(vector<DEVICE>& arrDevices);
 
    /**
     * GetMice
     * get all attached mouse devices
     * @param arrDevices reference to array of devices, filled by function
     * @return number of devices added to 'arrDevices'
     */
    int GetMice(vector<DEVICE>& arrDevices);
 
    /**
     * GetInput
     * @return pointer to direct input object
     */
    inline LPDIRECTINPUT8 GetInput() { return m_pDI; }
 
    /**
     * GetWindow
     * @return window handle used to initialize the input engine
     */
    inline HWND GetWindow() { return m_hWnd; }
 
private:
    /**
     * EnumDevicesCallback
     * static callback is used by direct input to notify us
     * about found input devices
     */
    static int WINAPI EnumDevicesCallback(  LPCDIDEVICEINSTANCE pInstance, 
                                            void* pParameters);
    //the direct input object
    LPDIRECTINPUT8        m_pDI;
 
    //application main window
    HWND            m_hWnd;
};