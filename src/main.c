/**
* Simple & Stupid Filesystem.
*
* Mohammed Q. Hussain - http://www.maastaar.net
*
* This is an example of using FUSE to build a simple filesystem. It is a part of a tutorial in MQH Blog with the title "Writing a Simple Filesystem Using FUSE in C": http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
*
* License: GNU GPL
*/

#define FUSE_USE_VERSION 30
#define _FILE_OFFSET_BITS 64

#include "./../include/functions.h"

#define last_two_numbers_of_student_card 36

char readme_txt[200]; 
char example_txt[200];
char test_txt[50] = "Welcome to my module\n";


static void init_module()
{
	strcpy(readme_txt,"Student <Graskov Viktor>, 1823236");
 	strcpy( example_txt, "Hello world! Student Graskov Viktor, group 13, task 4");
}


static int do_getattr( const char *path, struct stat *st )
{
   printf( "[getattr] Called\n" );
   printf( "\tAttributes of %s requested\n", path );

   // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
   //        st_uid:     The user ID of the file’s owner.
   //        st_gid:     The group ID of the file.
   //        st_atime:     This is the last access time for the file.
   //        st_mtime:     This is the time of the last modification to the contents of the file.
   //        st_mode:     Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
   //        st_nlink:     The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
   //                        as no process still holds it open. Symbolic links are not counted in the total.
   //        st_size:    This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

   st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
   st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
   st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
   st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

   int size = 0;

   memset(st, 0, sizeof(struct stat));

   if (strcmp(path, "/") == 0) {
       st->st_mode = S_IFDIR | 0700;
       st->st_nlink = 2;
   }
   else if (strcmp(path, "/bin") == 0){
       st->st_mode = S_IFDIR | 0700;
       st->st_nlink = 3;
   }
   else if (strcmp(path, "/bin/date") == 0){
       st->st_mode = S_IFREG | 0677;
       st->st_nlink = 1;

       struct stat buffer;
       stat("/bin/date", &buffer);
       st->st_size = buffer.st_size;
   }

   else if (strcmp(path, "/foo") == 0 ){
       st->st_mode = S_IFDIR | 441;
       st->st_nlink = 2;
   }
   else if (strcmp(path, "/foo/test.txt") == 0){
       st->st_mode = S_IFREG | 0000;
       st->st_nlink = 1;

       for(int i=0; i < last_two_numbers_of_student_card ; i++) {
           size += strlen(test_txt);
       }
       st->st_size = size;
   }

   else if (strcmp(path, "/foo/bar") == 0){
       st->st_mode = S_IFDIR | 0664;
       st->st_nlink = 0;
   }
   else if (strcmp(path, "/foo/baz") == 0){
       st->st_mode = S_IFDIR | 0244;
       st->st_nlink = 3;
   }

   else if (strcmp(path, "/foo/example") == 0){
       st->st_mode = S_IFREG | 0200;
       st->st_nlink = 1;

       st->st_size = strlen(example_txt);
   }
   else if (strcmp(path, "/foo/baz/readme.txt") == 0){
       st->st_mode = S_IFREG | 0441;
       st->st_nlink = 1;

       st->st_size = strlen(readme_txt);
   }
   else{
       return -1;
   }

   return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
   printf( "--> Getting The List of Files of %s\n", path );


   (void) offset;
   (void) fi;

   if (strcmp(path, "/") == 0) {
       filler(buffer, ".", NULL, 0);
       filler(buffer, "..", NULL, 0);
       filler(buffer, "foo", NULL, 0);
       filler(buffer, "bin", NULL, 0);
       return 0;
   }
   if (strcmp(path, "/bin") == 0) {
       filler(buffer, ".", NULL, 0);
       filler(buffer, "..", NULL, 0);
       filler(buffer, "date", NULL, 0);
       return 0;
   }
   if (strcmp(path, "/foo/bar") == 0) {
       filler(buffer, ".", NULL, 0);
       filler(buffer, "..", NULL, 0);
       return 0;
   }
   if (strcmp(path, "/foo/baz") == 0) {
       filler(buffer, ".", NULL, 0);
       filler(buffer, "..", NULL, 0);
       filler(buffer, "readme.txt", NULL, 0);
       return 0;
   }
   if (strcmp(path, "/foo") == 0 ) {
       filler(buffer, ".", NULL, 0);
       filler(buffer, "..", NULL, 0);
       filler(buffer, "baz", NULL, 0);
       filler(buffer, "bar", NULL, 0);
       filler(buffer, "example", NULL, 0);
       filler(buffer, "test.txt", NULL, 0);
       return 0;
   }

   return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
   printf( "--> Trying to read %s, %u, %u\n", path, offset, size );

   size_t len;
   (void) fi;

   char *fileBuffer;
   char tmp[last_two_numbers_of_student_card*55];

   if (strcmp(path, "/bin/date") == 0) {

       time_t t = time(NULL);
       struct tm tm = *localtime(&t);
       char te[50];
       sprintf( te, "now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
       len = strlen(te);
       fileBuffer = te;

   }
   else if (strcmp(path, "/foo/test.txt") == 0) {
       strcpy(tmp, "");
       for(int i=0; i<last_two_numbers_of_student_card; i++) {
           len += strlen(test_txt);
           strcat(tmp, test_txt);
           strcat(tmp, "\n");
       }
       fileBuffer = tmp;
   }
   else if (strcmp(path, "/foo/baz/readme.txt") == 0) {
       len = strlen(readme_txt);
       fileBuffer = readme_txt;
       printf("printf in read %s", fileBuffer);
   }
   else if (strcmp(path, "/foo/example") == 0) {
       len = strlen(example_txt);
       fileBuffer = example_txt;
       //printf("printf in read %s", fileBuffer);
   }
   else{
       return 0;
   }

   if (offset < len){
       if (offset + size > len){
           size = len-offset;
       }
       memcpy(buffer, fileBuffer+offset, size);
       return size;
   }
   else{
       printf("printf in read not now %s", fileBuffer);
       return 0;
   }

}

static int do_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
  
     printf( "--> Trying to write %s, %u, %u\n", path, offset, size );
     
   if (strcmp(path, "/bin/date") == 0) {
       return 0;
   }
   else if (strcmp(path, "/foo/test.txt") == 0) {
       return 0;
   }
   else if (strcmp(path, "/foo/baz/readme.txt") == 0) {
       strcpy(readme_txt, buf);
       return strlen(buf);
   }
   else if (strcmp(path, "/foo/example") == 0) {
       strcpy(example_txt, buf);
       return strlen(buf);
   }
   else{
       return 0;
   }
}

static struct fuse_operations operations = {
       .getattr    = do_getattr,
       .readdir    = do_readdir,
       .read        = do_read,
       .write      =do_write,
};

int main( int argc, char *argv[] )
{
	init_module();
	return fuse_main( argc, argv, &operations, NULL );
}










