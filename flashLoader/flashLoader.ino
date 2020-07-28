// SPI Flash CLI interface
//
//
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

// up to 11 characters
#define DISK_LABEL    "EXT_FLASH"

// Since SdFat doesn't fully support FAT12 such as format a new flash
// We will use Elm Cham's fatfs f_mkfs() to format
#include "ff.h"
#include "diskio.h"

// Include xmodem file transfer protocol
#include "xmodem.h"

FATFS elmchamFatfs;
uint8_t workbuf[4096]; // Working buffer for f_fdisk function.

// On-board external flash (QSPI or SPI) macros should already
// defined in your board variant if supported
// - EXTERNAL_FLASH_USE_QSPI
// - EXTERNAL_FLASH_USE_CS/EXTERNAL_FLASH_USE_SPI
#if defined(EXTERNAL_FLASH_USE_QSPI)
  Adafruit_FlashTransport_QSPI flashTransport;
#elif defined(EXTERNAL_FLASH_USE_SPI)
  Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
#else
  #error No QSPI/SPI flash are defined on your board variant.h !
#endif

Adafruit_SPIFlash flash(&flashTransport);
const SPIFlash_Device_t flashDevice = EXTERNAL_FLASH_DEVICE;

// file system object from SdFat
FatFileSystem fatfs;

// CLI Interface defines and variables
#define LINE_BUF_SIZE 128   // Maximum input string length
#define ARG_BUF_SIZE 64     // Maximum argument string length
#define MAX_NUM_ARGS 8      // Maximum number of arguments
 
bool error_flag = false;
char line[LINE_BUF_SIZE];
char args[MAX_NUM_ARGS][ARG_BUF_SIZE];

#define SERIAL_READ_TIMEOUT 30000 // 30 seconds
int _inbyte(unsigned short t);
void _outbyte(int c);

//List of functions pointers corresponding to each command
int (*commands_func[])(){
    &cmd_help,
    &cmd_format,
    &cmd_write,
    &cmd_delete,
    &cmd_dir,
    &cmd_mkdir,
    &cmd_rmdir
};

//List of command names
const char *commands_str[] = {
    "help",
    "format",
    "write",
    "delete",
    "dir",
    "mkdir",
    "rmdir"
};

int num_commands = sizeof(commands_str) / sizeof(char *);


void setup() {
  // Initialize serial port and wait for it to open before continuing.
  Serial.begin(115200);
  while (!Serial) delay(100);
  
  Serial.println("");
  Serial.println("");
  Serial.println("SPI Flash CLI Interface");

  // Set Power Enable Pin
  pinMode(PIN_POWER_ENABLE, OUTPUT);
  digitalWrite(PIN_POWER_ENABLE, HIGH);

  // Set Flash Reset# Pin
  pinMode(EXTERNAL_FLASH_RST_NOT, OUTPUT);
  digitalWrite(EXTERNAL_FLASH_RST_NOT, HIGH);
  
  // Initialize flash library and check its chip ID.
  if (!flash.begin(&flashDevice)) {
    Serial.println("Error, failed to initialize flash chip!");
    while(1) yield();
  }
  Serial.setTimeout(SERIAL_READ_TIMEOUT);
  Serial.print("- Flash chip JEDEC ID: 0x"); Serial.println(flash.getJEDECID(), HEX);
  
  if (!fatfs.begin(&flash)) {
    Serial.println("Failed to mount filesystem, format flash!");
  }
}

void loop() {
  cli();
}

void cli(){
  Serial.print("> ");

  read_line();
  if(!error_flag){
    parse_line();
  }
  if(!error_flag){
    execute();
  }

  memset(line, 0, LINE_BUF_SIZE);
  memset(args, 0, sizeof(args[0][0]) * MAX_NUM_ARGS * ARG_BUF_SIZE);

  error_flag = false;
}

void read_line(){
  String line_string;

  while(!Serial.available());

  if(Serial.available()){
    line_string = Serial.readStringUntil('\r');
    if(line_string.length() < LINE_BUF_SIZE){
      line_string.toCharArray(line, LINE_BUF_SIZE);
      Serial.println(line_string);
    }
    else{
      Serial.println("Input string too long.");
      error_flag = true;
    }
  }
}
 
void parse_line(){
  char *argument;
  int counter = 0;

  argument = strtok(line, " ");

  while((argument != NULL)){
    if(counter < MAX_NUM_ARGS){
      if(strlen(argument) < ARG_BUF_SIZE){
        strcpy(args[counter],argument);
        argument = strtok(NULL, " ");
        counter++;
      }
      else{
        Serial.println("Input string too long.");
        error_flag = true;
        break;
      }
    }
    else{
      break;
    }
  }
}
 
int execute(){  
  for(int i=0; i<num_commands; i++){
    if(strcmp(args[0], commands_str[i]) == 0){
      return(*commands_func[i])();
    }
  }

  Serial.println("Invalid command. Type \"help\" for more.");
  return 0;
}

int cmd_help(){
  Serial.println("The following commands are available:");

  for(int i=0; i<num_commands; i++){
    Serial.print("  ");
    Serial.println(commands_str[i]);
  }
  Serial.println("");
}

int cmd_format() {
  Serial.println("Creating and formatting FAT filesystem...");

  // Make filesystem.
  FRESULT r = f_mkfs("", FM_FAT | FM_SFD, 0, workbuf, sizeof(workbuf));
  if (r != FR_OK) {
    Serial.print("Error, f_mkfs failed with error code: "); Serial.println(r, DEC);
    while(1) yield();
  }

  // mount to set disk label
  r = f_mount(&elmchamFatfs, "0:", 1);
  if (r != FR_OK) {
    Serial.print("Error, f_mount failed with error code: "); Serial.println(r, DEC);
    while(1) yield();
  }

  Serial.print("fs_type:   "); Serial.println(elmchamFatfs.fs_type, DEC);
  Serial.print("pdrv:      "); Serial.println(elmchamFatfs.pdrv, DEC);
  Serial.print("n_fats:    "); Serial.println(elmchamFatfs.n_fats, DEC);
  Serial.print("n_rootdir: "); Serial.println(elmchamFatfs.n_rootdir, DEC);
  Serial.print("csize:     "); Serial.println(elmchamFatfs.csize, DEC);
  Serial.print("last_clst: "); Serial.println(elmchamFatfs.last_clst, DEC);
  Serial.print("n_fatent:  "); Serial.println(elmchamFatfs.n_fatent, DEC);
  Serial.print("fsize:     "); Serial.println(elmchamFatfs.fsize, DEC);

  // Setting label
  Serial.println("Setting disk label to: " DISK_LABEL);
  r = f_setlabel(DISK_LABEL);
  if (r != FR_OK) {
    Serial.print("Error, f_setlabel failed with error code: "); Serial.println(r, DEC);
    while(1) yield();
  }

  // unmount
  f_unmount("0:");

  // sync to make sure all data is written to flash
  flash.syncBlocks();
  
  Serial.println("Formatted flash!");

  // Check new filesystem
  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    while(1) delay(1);
  }

  // Done!
  Serial.println("Filesystem Mounted");
}

int cmd_write(){
  if (strlen(args[1]) == 0){
    Serial.println("Error, Invalid file/path");
  }
  else{
    if (!fatfs.exists(args[1])) {
      File writeFile = fatfs.open(args[1], FILE_WRITE);
      if (!writeFile) {
        Serial.println("Error, failed to create file!");
      }
      else{
        Serial.println("Ready for file to be transferred...");
        uint32_t fileSize = 0;

        // Receive file via xmodem protocol
        int sts = XmodemReceiveCrc(&xmodem_store_chunk, &writeFile, 33554432);
        if (sts > 0)
        {
          if (writeFile.getWriteError()){
            Serial.println("");
            Serial.println("Error, File System Error");
            writeFile.close();
            fatfs.remove(args[1]);
          }
          else{
            Serial.println("");
            Serial.println("File Transferred");
            
            // Read file size
            fileSize = writeFile.size();
            writeFile.close();
          }
        }
        else {
          Serial.print("Error, xmodem File Transfer Error, ");
          Serial.println(sts, DEC);
          writeFile.close();
          fatfs.remove(args[1]);
        }

        // Reset read timeout
        Serial.setTimeout(SERIAL_READ_TIMEOUT);

        if (!fatfs.exists(args[1]) ) {
          Serial.print("Error, failed to create file ");
          Serial.println(args[1]);
        }else {
          Serial.print("Saved file ");
          Serial.print(args[1]);
          Serial.print(" - ");
          Serial.print(fileSize, DEC);
          Serial.println("B");
        }
      }
    }
    else{
      Serial.print("Error, ");
      Serial.print(args[1]);
      Serial.println(" already exists");
    }
  }
}

int cmd_delete(){
  if (strlen(args[1]) == 0){
    Serial.println("Invalid file/path");
  }
  else{
    if (!fatfs.remove(args[1])) {
      Serial.print("Error, couldn't delete ");
    }
    else{
      Serial.print("Deleted ");
    }
    Serial.println(args[1]);
  }
}

int cmd_dir(){
  if (strlen(args[1]) == 0){
    Serial.println("Invalid path");
  }
  else{
    File tempDir = fatfs.open(args[1]);
    if (!tempDir) {
      Serial.print("Error, failed to open directory ");
      Serial.println(args[1]);
    }
    else{
      if (!tempDir.isDirectory()) {
        Serial.println("Error, expected a directory!");
      }
      else{
        Serial.println("Listing children of directory:");
        File child = tempDir.openNextFile();
        while (child) {
          char filename[64];
          child.getName(filename, sizeof(filename));
          
          // Print the file name and file size or if it's a directory.
          Serial.print("- ");
          Serial.print(filename);
          if (child.isDirectory()) {
            Serial.println(" (directory)");
          }
          else{
            Serial.print(" ");
            Serial.print(child.size(), DEC);
            Serial.println("B");
          }

          // Keep calling openNextFile to get a new file
          child = tempDir.openNextFile();
        }
      }
    }
  }
}

int cmd_mkdir(){
  if (strlen(args[1]) == 0){
    Serial.println("Error, Invalid path");
  }
  else{
    if (!fatfs.exists(args[1])) {
      
      // Note: You should NOT have a trailing slash
      fatfs.mkdir(args[1]);
      
      if (!fatfs.exists(args[1]) ) {
        Serial.print("Error, failed to create directory ");
      }else {
        Serial.print("Created directory ");
      }
      Serial.println(args[1]);
    }
    else{
      Serial.print("Error, ");
      Serial.print(args[1]);
      Serial.println(" already exists");
    }
  }
}

int cmd_rmdir(){
  if (strlen(args[1]) == 0){
    Serial.println("Invalid path");
  }
  else{
    if (fatfs.exists(args[1])) {
      
      // Note: You should NOT have a trailing slash
      fatfs.rmdir(args[1]);
      
      if (fatfs.exists(args[1]) ) {
        Serial.print("Error, failed to remove directory ");
      }else {
        Serial.print("Removed directory ");
      }
      Serial.println(args[1]);
    }
    else{
      Serial.print(args[1]);
      Serial.println(" does not exist");
    }
  }
}

int _inbyte(unsigned short t){
  Serial.setTimeout(t);
  char byte = 0;
  Serial.readBytes(&byte, 1);
  return (int)byte;
}

void _outbyte(int c){
  Serial.write(c);
}

void xmodem_store_chunk(
  /* Pointer to the function context (can be used for anything) */
  void *funcCtx,
  /* Pointer to the XMODEM receive buffer (store data from here) */
  void *xmodemBuffer,
  /* Number of bytes received in the XMODEM buffer (and to be stored) */
  int xmodemSize){
  
  File *writeFile = (File *)funcCtx;
  writeFile->write(xmodemBuffer, xmodemSize);
}

//--------------------------------------------------------------------+
// fatfs diskio
//--------------------------------------------------------------------+
extern "C"
{

DSTATUS disk_status ( BYTE pdrv )
{
  (void) pdrv;
	return 0;
}

DSTATUS disk_initialize ( BYTE pdrv )
{
  (void) pdrv;
	return 0;
}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
  (void) pdrv;
	return flash.readBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
  (void) pdrv;
  return flash.writeBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  (void) pdrv;

  switch ( cmd )
  {
    case CTRL_SYNC:
      flash.syncBlocks();
      return RES_OK;

    case GET_SECTOR_COUNT:
      *((DWORD*) buff) = flash.size()/512;
      return RES_OK;

    case GET_SECTOR_SIZE:
      *((WORD*) buff) = 512;
      return RES_OK;

    case GET_BLOCK_SIZE:
      *((DWORD*) buff) = 8;    // erase block size in units of sector size
      return RES_OK;

    default:
      return RES_PARERR;
  }
}

}