#include <windows.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <string>

using namespace std;

HANDLE open_com(uint8_t num)
{
  std::string port = "\\\\.\\COM8";
  HANDLE hComm = CreateFile("\\\\.\\COM8", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (hComm == INVALID_HANDLE_VALUE)
  {
    throw "Error in opening serial port";
  }

  /*
  81 10 F1 81 03 -> 83F110C16B8F3F
83 10 F1 10 81 0A 1F -> 82F110508154
8210F13E01C2 -> 81F1107E00
8410F1300F0601CB -> 84F110700F06010B
8510F12300F30008A4 -> 80F11009630000000000000000ED
8510F1230000C01079 -> 80F11011632672BAD31EBFE4A5153F2108BD9C7B6A3B
8110F120A2 -> 81F11060E2
8110F18204 -> 81F110C244
  */

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  GetCommState(hComm, &dcbSerialParams);
  dcbSerialParams.BaudRate = CBR_38400;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  SetCommState(hComm, &dcbSerialParams);

  //SetCommMask(hComm, EV_RXCHAR);

  return hComm;
}

uint8_t calc_checksum(uint8_t *data, size_t len)
{
  return 0;
}

void wait4data(HANDLE hComm)
{
  DWORD dwEventMask;
  WaitCommEvent(hComm, &dwEventMask, NULL);
  uint8_t TempChar;          //Temporary character used for reading
  uint8_t SerialBuffer[256]; //Buffer for storing Rxed Data
  DWORD NoBytesRead;
  int i = 0;

  do
  {
    ReadFile(hComm,            //Handle of the Serial port
             &TempChar,        //Temporary character
             sizeof(TempChar), //Size of TempChar
             &NoBytesRead,     //Number of bytes read
             NULL);

    SerialBuffer[i] = TempChar; // Store Tempchar into buffer
    printf("0x%02X ", TempChar);
    i++;
  } while (NoBytesRead > 0);
  puts("");
}

int main()
{

  DWORD bw = 0;

  HANDLE hComm = open_com(8);

  const uint8_t startCommunication[] = {0x81, 0x10, 0xf1, 0x81, 0x03};

  const uint8_t readDataByLocalIdentifier_RLI_ASS[] = {0x82, 0x10, 0xF1, 0x21, 0x01, 0xA5};

  if (!WriteFile(hComm, startCommunication, sizeof(startCommunication), &bw, NULL))
  {
    printf("Failed to write byte %u", GetLastError());
  }
  wait4data(hComm);

  CloseHandle(hComm); //Closing the Serial Port
  return 0;
}