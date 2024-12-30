#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/**
 * Sources:
 *
 * For readdir:
 *      https://pubs.opengroup.org/onlinepubs/7908799/xsh/readdir.html#:~:text=The%20readdir()%20function%20returns,end%20of%20the%20directory%20stream.
 *      https://man7.org/linux/man-pages/man3/readdir.3.html
 * For getting directory entry file type and other dirent info:
 *      https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
 * For getting user ID:
 *      https://pubs.opengroup.org/onlinepubs/009604499/functions/getpwuid.html
 * For getting group ID:
 *      https://pubs.opengroup.org/onlinepubs/009604499/functions/getgrgid.html
 * For deciphering file type within the stat structure:
 *      https://aljensencprogramming.wordpress.com/tag/st_mode/
 *      This source also wrote about using the & operator in combination with the bits, which made me listen to a Stack Overflow post I would've otherwise ignored.
 * For the permission mode bits:
 *      https://pubs.opengroup.org/onlinepubs/007904875/basedefs/sys/stat.h.html
 * For converting the permission bits into human-readable letters:
 *      https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
 * For strcpy:
 *      https://www.programiz.com/c-programming/library-function/string.h/strcpy
 * For strcmp:
 *      https://www.programiz.com/c-programming/library-function/string.h/strcmp
 * For strcat:
 *      https://www.geeksforgeeks.org/concatenating-two-strings-in-c/
 */

void list_directory(const char* directory_name);

int main(int argc, char** argv) {
  // Process arguments
  if (argc == 1) {
    list_directory(".");
  } else if (argc == 2) {
    list_directory(argv[1]);
  } else {
    fprintf(stderr, "Please specify one or zero command line arguments.\n");
  }

  return 0;
}

void list_directory(const char* directory_name) {
  //open directory and save it into directory variable
  DIR * directory = opendir(directory_name);

  //handle case where given directory doesn't exist
  if(directory==NULL){
    fprintf(stderr, "Failed to open directory: no such file or directory\n");
  }else{
    //store first directory entry into entry
    struct dirent * entry = readdir(directory);

    //to hold the entry as a stat type
    struct stat holder;

    //loop through all entries in directory
    while(entry!=NULL){
      char filepath[100];
      if(strcmp(directory_name, ".") !=0){
        strcpy(filepath, directory_name);
        strcat(filepath, "/");
        strcat(filepath, entry->d_name);
      }else{
        strcpy(filepath, entry->d_name);
      }

      //store entry into stat type called holder
      stat(filepath, &holder);

      //print file type as either directory (d) or (-)
      if(S_ISDIR(holder.st_mode)){
        printf("d");
      }else{
        printf("-");
      }//else

      /**
       * I know the following code is ugly. I saw an AI version that's
       * clearner, but this work is my own and I'd rather submit that.
       * In the future, I will use the better structure.
       * Better Structure:
       * printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
       */


      //user reading
      if(holder.st_mode & S_IRUSR){
        printf("r");
      }else{
        printf("-");
      }

      //user writing
      if(holder.st_mode & S_IWUSR){
        printf("w");
      }else{
        printf("-");
      }

      //user executing
      if(holder.st_mode & S_IXUSR){
        printf("x");
      }else{
        printf("-");
      }

      //group reading
      if(holder.st_mode & S_IRGRP){
        printf("r");
      }else{
        printf("-");
      }

      //group writing
      if(holder.st_mode & S_IWGRP){
        printf("w");
      }else{
        printf("-");
      }

      //group executing
      if(holder.st_mode & S_IXGRP){
        printf("x");
      }else{
        printf("-");
      }

      //anyone reading
      if(holder.st_mode & S_IROTH){
        printf("r");
      }else{
        printf("-");
      }

      //anyone writing
      if(holder.st_mode & S_IWOTH){
        printf("w");
      }else{
        printf("-");
      }

      //anyone executing
      if(holder.st_mode & S_IXOTH){
        printf("x");
      }else{
        printf("-");
      }

      //get & print owner's name (these two lines I wrote using one of the cited sources)
      struct passwd * owner = getpwuid(holder.st_uid);
      printf(" %s ", owner->pw_name);

      //get & print group id (this block is pasted from a website I cited and modified for spacing)
      struct group * grp;
      if ((grp = getgrgid(holder.st_gid)) != NULL){
        printf("%s ", grp->gr_name);
      } else {
        printf("%d ", holder.st_gid);
      }

      //get & print file size
      int filesize = (int) holder.st_size;
      printf("%d ", filesize);


      printf("%s\n",entry->d_name); //file name
      entry=readdir(directory);
    }

  }
}