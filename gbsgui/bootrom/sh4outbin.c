/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided AS IS, WITH ALL FAULTS. ST does not   */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights, trade secrets or other intellectual property rights.   */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**
 * @file     sh4outbin.c
 * @brief    Extract infos from .out file for image flash generation
 * @author   STMicroelectronics
 */

/* Include */
/* ------- */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Environment */
/* ----------- */
#if defined(MSDOS)
#define  sh4outbin_msdos
#define  new_gzip
#endif
#if defined(CYGWIN)
#define  sh4outbin_cygwin
#endif
#if defined(LINUX)
#define  sh4outbin_linux
#endif
#if defined(VMLINUX)
#define  sh4outbin_vmlinux
#endif

/* Global variables */
/* ---------------- */
static unsigned int   image_is_compressed            = 0;          /* Is set to 1 if image is compressed     */
static unsigned int   marker_is_present              = 0;          /* Is set to 1 if sdk marker to be placed */
static unsigned int   load_address                   = 0;          /* Where to load the code                 */
static unsigned int   image_size                     = 0;          /* Image size                             */
static unsigned int   image_compressed_address_zlib  = 0x00020080; /* Image compressed start address         */
static unsigned int   image_compressed_address       = 0x00004080; /* Image compressed start address         */
static unsigned int   image_compressed_size          = 0;          /* Image compressed size                  */
static unsigned int   start_address                  = 0;          /* User code entry point                  */
static unsigned int   stack_address                  = 0;          /* Stack address                          */
static unsigned int   debug_connected_address        = 0;          /* Debug address                          */
static unsigned char  start_image_header0[]          = "_GBS_ST_IRD_DVBT2_APPLICATION_\0"; /* An extra 0 is added at the end of the string */
static unsigned char  start_image_header1[]          = "_VERSION_XXXXXXXXXXXXXXXXXXXX_\0"; /* An extra 0 is added at the end of the string */
static unsigned char  start_image_header2[]          = "_RELEASE_DD_MM_YYYY___________\0"; /* An extra 0 is added at the end of the string */
static unsigned char  start_image_header3[]          = "_SIGNATURE____________________\0"; /* An extra 0 is added at the end of the string */

/* ========================================================================
   Name:        get_field
   Description: Return a field in a string content
   ======================================================================== */

static char *get_field(char *command,int match_field,char *match_string,int return_field)
{
 char  buffer[256];
 FILE *pipe;
 char *ptr;
 char *fields[16];
 int   i;
 char *return_string=NULL;

 /* Make field numbering zero based */
 /* ------------------------------- */
 match_field--;
 return_field--;

 /* Exec cmd and capture output via a pipe */
 /* -------------------------------------- */
 pipe=popen(command,"r");
 if (pipe==NULL)
  {
   return(NULL);
  }

 /* Parse all the lines */
 /* ------------------- */
 while(!feof(pipe))
  {
   if (fgets(buffer,sizeof(buffer),pipe))
    {
     ptr=strtok(buffer," \t\n\r");
     for(i=0;((i<16)&&(ptr!=NULL));i++)
      {
       fields[i]=ptr;
       ptr=strtok(NULL," \t\n\r");
      }
     if((match_field<i)&&(return_field<i))
      {
       if (strcmp(fields[match_field],match_string)==0)
        {
         return_string=strdup(fields[return_field]);
         /* We don't close the pipe to avoid to flush the pipe which take too long time */
         /* pclose(pipe) */
         return(return_string);
        }
      }
    }
  }

 /* Close the pipe */
 /* -------------- */
 pclose(pipe);

 /* Return string */
 /* ------------- */
 return(return_string);
}

/* ========================================================================
   Name:        main
   Description: main is MAIN. Entry point of the application.
   ======================================================================== */

int main(int argc,char* argv[])
{
 char          system_command[256];
 char         *image_name;
 char         *ptr;
 char         *arg_ptr;
 struct stat   stat_buf;
 FILE         *file;
 unsigned int  i;
 unsigned int  sh4_linux_gcc_version;

 /* Check arguments */
 /* --------------- */
 if ((argc<3)||(argc>5))
  {
   fprintf(stderr,"Usage: %s <main.out> <main.bin> [-z] [-nomarker]\n",argv[0]);
   fprintf(stderr," -z        : compress the binary image file \n");
   fprintf(stderr," -nomarker : don't set the STAPI_SDK marker in the rom image \n");
   fprintf(stderr,"(c) STMicroElectronics 2010 - V1.3\n");
   exit(-1);
  }

 /* Look for parameters */
 /* ------------------- */
 image_is_compressed = 0;
 marker_is_present   = 1;
 for (i=0;i<argc;i++)
  {
   if ((arg_ptr=strstr(argv[i],"-z"))!=NULL)
    {
     image_is_compressed=1;
    }
   if ((arg_ptr=strstr(argv[i],"-nomarker"))!=NULL)
    {
     marker_is_present=0;
    }
  }
 if (marker_is_present==1)
  {
   image_compressed_address_zlib = image_compressed_address_zlib + (4 * sizeof(start_image_header0));
   image_compressed_address      = image_compressed_address + (4 * sizeof(start_image_header0));
  }

 /* Convert the .out file in .bin file */
 /* ---------------------------------- */
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"sh4-linux-objcopy -O binary %s m.bin",argv[1]);
#else
 sprintf(system_command,"sh4objcopy -O binary -R .stack %s m.bin",argv[1]);
#endif
 if (system(system_command)!=0)
  {
   fprintf(stderr,"--> Unable to create the binary file of the application !\n");
   exit(-1);
  }
 if (stat("m.bin",&stat_buf))
  {
   fprintf(stderr,"--> Unable to get the file size of the %s file !\n",argv[2]);
   exit(-1);
  }
 image_size=stat_buf.st_size;

 /* Compress the bin file */
 /* --------------------- */
 if (image_is_compressed)
  {
#if defined(sh4outbin_msdos)
   sprintf(system_command,"gzip >NUL: 2>NUL: m.bin");
#endif
#if defined(sh4outbin_cygwin)
   sprintf(system_command,"/bin/gzip 2>/dev/null m.bin");
#endif
#if defined(sh4outbin_linux)
   sprintf(system_command,"gzip 2>/dev/null m.bin");
#endif
#if defined(sh4outbin_vmlinux)
   sprintf(system_command,"gzip 2>/dev/null m.bin");
#endif
   if (system(system_command)!=0)
    {
     fprintf(stderr,"--> Unable to compress the application !\n");
     exit(-1);
    }
#if defined(sh4outbin_msdos)
#if defined(new_gzip)
   if (stat("m.bin.gz",&stat_buf))
#else
   if (stat("m.biz",&stat_buf))
#endif
#endif
#if defined(sh4outbin_cygwin)
   if (stat("m.bin.gz",&stat_buf))
#endif
#if defined(sh4outbin_linux)
   if (stat("m.bin.gz",&stat_buf))
#endif
#if defined(sh4outbin_vmlinux)
   if (stat("m.bin.gz",&stat_buf))
#endif
    {
     fprintf(stderr,"--> Unable to get the compressed file !\n");
     exit(-1);
    }
   image_compressed_size=stat_buf.st_size;
  }

 /* Adjust final size if odd */
 /* ------------------------ */
 if (image_is_compressed)
  {
   if ((image_compressed_size&0x3)!=0)
    {
     file=fopen("dummy.bin","wb");
     if (file==NULL)
      {
       fprintf(stderr,"--> Unable to create a dummy file !\n");
       exit(-1);
      }
     fwrite(&file,1,4-(image_compressed_size&0x3),file);
     fclose(file);
#if defined(sh4outbin_msdos)
#if defined(new_gzip) /*DC Addition DC*/
     sprintf(system_command,"copy >NUL: 2>NUL: /B m.bin.gz+dummy.bin m.zip");
#else
     sprintf(system_command,"copy >NUL: 2>NUL: /B m.biz+dummy.bin m.zip");
#endif
#endif
#if defined(sh4outbin_cygwin)
     sprintf(system_command,"cat m.bin.gz dummy.bin >m.zip");
#endif
#if defined(sh4outbin_linux)
     sprintf(system_command,"cat m.bin.gz dummy.bin >m.zip");
#endif
#if defined(sh4outbin_vmlinux)
     sprintf(system_command,"cat m.bin.gz dummy.bin >m.zip");
#endif
     if (system(system_command)!=0)
      {
       fprintf(stderr,"--> Unable to merge the headers in the main binary file !\n");
       exit(-1);
      }
    }
  }

 /* Get load address */
 /* ---------------- */
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"sh4-linux-objdump -h %s",argv[1]);
#else
 sprintf(system_command,"sh4objdump -h %s",argv[1]);
#endif
 ptr = get_field(system_command,1,"0",4);
 load_address = strtoul(ptr,NULL,16);
 printf("LoadAddress            = 0x%08x\n",load_address);
 printf("ImageSize              = 0x%08x\n",image_size);
 if (image_is_compressed)
  {
   printf("ImageCompressedAddress = 0x%08x\n",image_compressed_address_zlib);
  }
 else
  {
   printf("ImageCompressedAddress = 0x%08x\n",image_compressed_address);
  }
 printf("ImageCompressedSize    = 0x%08x\n",image_compressed_size);

 /* Get start address */
 /* ----------------- */
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"sh4-linux-objdump -f %s",argv[1]);
#else
 sprintf(system_command,"sh4objdump -f %s",argv[1]);
#endif
 ptr = get_field(system_command,1,"start",3);
 start_address = strtoul(ptr,NULL,16);
 printf("StartAddress           = 0x%08x\n",start_address);

 /* Get stack address */
 /* ----------------- */
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"sh4-linux-gcc -v 2>&1");
 ptr = get_field(system_command,2,"version",3);
 sh4_linux_gcc_version = atoi(ptr)*100+atoi(ptr+2)*10+atoi(ptr+4);
 sprintf(system_command,"sh4-linux-objdump -d %s",argv[1]);
 if (sh4_linux_gcc_version>=444)
  {
   ptr = get_field(system_command,9,"<__nosave_begin>",8);
  }
 else
  {
   ptr = get_field(system_command,9,"<_ebss>",8);
  }
 stack_address = strtoul(ptr,NULL,16);
 printf("StackAddress           = 0x%08x\n",stack_address);
#else
 sprintf(system_command,"sh4objdump -d %s",argv[1]);
 ptr = get_field(system_command,9,"<_stack>",8);
 stack_address = strtoul(ptr,NULL,16);
 printf("StackAddress           = 0x%08x\n",stack_address);
#endif

 /* Get debug connected address */
 /* --------------------------- */
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"sh4-linux-objdump -t %s",argv[1]);
#else
 sprintf(system_command,"sh4objdump -t %s",argv[1]);
#endif
 ptr = get_field(system_command,6,"__SH_DEBUGGER_CONNECTED",1);
 if (ptr==0)
  {
   debug_connected_address = 0x00000000;
  }
 else
  {
   debug_connected_address = strtoul(ptr,NULL,16);
  }
 printf("DebugConnectedAddress  = 0x%08x\n",debug_connected_address);

 /* Create header file containing adress/size infos */
 /* ----------------------------------------------- */
 file=fopen("h.bin","wb");
 if (file==NULL)
  {
   fprintf(stderr,"--> Unable to create a tmp file !\n");
   exit(-1);
  }
 fwrite(&load_address              ,1,sizeof(int),file);
 fwrite(&image_size                ,1,sizeof(int),file);
 if (image_is_compressed)
  {
   fwrite(&image_compressed_address_zlib,1,sizeof(int),file);
  }
 else
  {
   fwrite(&image_compressed_address,1,sizeof(int),file);
  }
 fwrite(&image_compressed_size     ,1,sizeof(int),file);
 fwrite(&start_address             ,1,sizeof(int),file);
 fwrite(&stack_address             ,1,sizeof(int),file);
 fwrite(&debug_connected_address   ,1,sizeof(int),file);
 if (marker_is_present==1)
  {
   fwrite(start_image_header0       ,1,sizeof(start_image_header0),file);
   fwrite(start_image_header1       ,1,sizeof(start_image_header1),file);
   fwrite(start_image_header2       ,1,sizeof(start_image_header2),file);
   fwrite(start_image_header3       ,1,sizeof(start_image_header3),file);
  }
 fclose(file);

 /* Merge header.bin & main.bin */
 /* --------------------------- */
 if (image_compressed_size==0) image_name="m.bin";
 else
  {
   if ((image_compressed_size&3)!=0) image_name="m.zip";
#if defined(sh4outbin_msdos)
#if defined(new_gzip) /*DC Addition DC*/
   else image_name="m.bin.gz";
#else
   else image_name="m.biz";
#endif
#endif
#if defined(sh4outbin_cygwin)
   else image_name="m.bin.gz";
#endif
#if defined(sh4outbin_linux)
   else image_name="m.bin.gz";
#endif
#if defined(sh4outbin_vmlinux)
   else image_name="m.bin.gz";
#endif
  }
#if defined(sh4outbin_msdos)
 sprintf(system_command,"copy >NUL: 2>NUL: /B h.bin+%s %s",image_name,argv[2]);
#endif
#if defined(sh4outbin_cygwin)
 sprintf(system_command,"cat h.bin %s >%s",image_name,argv[2]);
#endif
#if defined(sh4outbin_linux)
 sprintf(system_command,"cat h.bin %s >%s",image_name,argv[2]);
#endif
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"cat h.bin %s >%s",image_name,argv[2]);
#endif
 if (system(system_command)!=0)
  {
   fprintf(stderr,"--> Unable to merge the headers in the main binary file %s %s!\n",image_name,argv[2]);
   exit(-1);
  }

 /* Remove h.bin */
 /* ------------ */
#if defined(sh4outbin_msdos)
 sprintf(system_command,"del >NUL: 2>NUL: h.bin");
#endif
#if defined(sh4outbin_cygwin)
 sprintf(system_command,"rm 2>/dev/null h.bin");
#endif
#if defined(sh4outbin_linux)
 sprintf(system_command,"rm 2>/dev/null h.bin");
#endif
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"rm 2>/dev/null h.bin");
#endif
 system(system_command);

 /* Remove m.bin */
 /* ------------ */
#if defined(sh4outbin_msdos)
 sprintf(system_command,"del >NUL: 2>NUL: m.bin");
#endif
#if defined(sh4outbin_cygwin)
 sprintf(system_command,"rm 2>/dev/null m.bin");
#endif
#if defined(sh4outbin_linux)
 sprintf(system_command,"rm 2>/dev/null m.bin");
#endif
#if defined(sh4outbin_vmlinux)
 sprintf(system_command,"rm 2>/dev/null m.bin");
#endif
 system(system_command);

 /* Remove m.biz/m.bin.gz */
 /* --------------------- */
 if (image_compressed_size!=0)
  {
#if defined(sh4outbin_msdos)
#if defined(new_gzip) /*DC Addition DC*/
   sprintf(system_command,"del >NUL: 2>NUL: m.bin.gz");
#else
   sprintf(system_command,"del >NUL: 2>NUL: m.biz");
#endif
#endif
#if defined(sh4outbin_cygwin)
   sprintf(system_command,"rm 2>/dev/null m.bin.gz");
#endif
#if defined(sh4outbin_linux)
   sprintf(system_command,"rm 2>/dev/null m.bin.gz");
#endif
#if defined(sh4outbin_vmlinux)
   sprintf(system_command,"rm 2>/dev/null m.bin.gz");
#endif
   system(system_command);
  }

 /* Remove dummy.bin/m.zip */
 /* ---------------------- */
 if ((image_compressed_size&3)!=0)
  {
#if defined(sh4outbin_msdos)
   sprintf(system_command,"del >NUL: 2>NUL: dummy.bin");
#endif
#if defined(sh4outbin_cygwin)
   sprintf(system_command,"rm 2>/dev/null dummy.bin");
#endif
#if defined(sh4outbin_linux)
   sprintf(system_command,"rm 2>/dev/null dummy.bin");
#endif
#if defined(sh4outbin_vmlinux)
   sprintf(system_command,"rm 2>/dev/null dummy.bin");
#endif
   system(system_command);
#if defined(sh4outbin_msdos)
   sprintf(system_command,"del >NUL: 2>NUL: m.zip");
#endif
#if defined(sh4outbin_cygwin)
   sprintf(system_command,"rm 2>/dev/null m.zip");
#endif
#if defined(sh4outbin_linux)
   sprintf(system_command,"rm 2>/dev/null m.zip");
#endif
#if defined(sh4outbin_vmlinux)
   sprintf(system_command,"rm 2>/dev/null m.zip");
#endif
   system(system_command);
  }

 /* Quit */
 /* ---- */
 exit(0);
}


