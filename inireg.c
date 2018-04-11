
//inireg.c - simple INI vs Reg performance test
//cl inireg.c advapi32.lib

#include <stdio.h>
#include <time.h>
#include <windows.h>

int RegExportBranch(char *keystr1, char *keystr2, char *filename)
{
  char cmd[256];
  sprintf(cmd, "regedit /E %s %s\\%s", filename, keystr1, keystr2);
  return WinExec(cmd, SW_HIDE);
}

main()
{
  HKEY hkey;
  char *keystr = "Software\\Test";
  int i, numkey, bytes;
  time_t t1, t2;

  //first test .INI file access
  time(&t1);
  for(i=0, bytes=0; ; i++)
    {
      char key[256];
      char value[256];
      bytes += sprintf(key, "key_number_%u", i);
      time(&t2);
      bytes += sprintf(value, "value_number_%u (%lu)", i, t2);
      if (! WritePrivateProfileString("temp", key, value, "TMP.INI"))
	break; //probably exceeded 60-64k limit
    }
  time(&t2);
  numkey = i;
  printf("Private INI file:\n");
  printf("Wrote %u bytes in %u key/values\n", bytes, numkey);
  printf("%lu seconds\n\n", t2 - t1);

  //now test registry access
  time(&t1);
  if(RegCreateKey(HKEY_CURRENT_USER, keystr, &hkey) != ERROR_SUCESS)
    return 1;
  for(i=0, bytes=0; i<numkey; i++)
    {
      char keystr2[256], value[256];
      int valuelen;
      bytes += sprintf(keystr2, "key_number_%u", i);
      time(&t2);
      valuelen = sprintf(value, "value_number_%u (%lu)", i, t2);
      bytes += valuelen;
      if(RegSetValue(hkey, keystr2, REG_Sz, value, valuelen) != ERROR_SUCCESS)
	break;
    }
  RegCloseKey(hkey);
  time(&t2);
  printf("Registry:\n");
  printf("Wrote %u bytes in %u key/values\n", bytes, i);
  printf("%lu seconds\n\n", t2 - t1);

  //now test writing the whole 60-64k registry data to a .REG file
  time(&t1);
#define STRINGIZE(x) #x
  REgExportBranch(STRINGIZE(HKEY_CURRENT_USER), keystr, "TEST.REG");
  time(&t2);
  printf("%lu seconds to run REGEDIT to export branch\n", t2 - t1);
  return 0;
}

