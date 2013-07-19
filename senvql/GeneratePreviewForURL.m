#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#import <Cocoa/Cocoa.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
 Generate a preview for file
 
 This function's job is to create preview for designated file
 ----------------------------------------------------------------------------- */


NSString* get_x_path(NSString* base_path, CFStringRef uti ) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ([base_path UTF8String])) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      
      NSString* path = [base_path stringByAppendingPathComponent:
                        [[NSString stringWithCString:ent->d_name] lowercaseString]];
      printf ("Testing %s\n", [path UTF8String]);
      CFStringRef fileExtension = (__bridge CFStringRef) [path pathExtension];
      CFStringRef fileUTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, fileExtension, NULL);
      
      if (UTTypeConformsTo(fileUTI, uti)) {
        printf ("Found conforming: %s\n", [path UTF8String]);
        closedir (dir);
        return path;
      }
    }
    closedir (dir);
  }
  return nil;
  
}


OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
  NSURL* url_str = (__bridge NSURL*)url;
  NSString* file_path = [url_str path];
  const char* file_path_cstr = [file_path UTF8String];
  
  printf("::GeneratePreviewForURL: %s\n", file_path_cstr);
  
  NSFileManager *fileManager= [NSFileManager defaultManager];
  [fileManager createDirectoryAtPath:@"/tmp/preview"
         withIntermediateDirectories:YES
                          attributes:nil error:NULL];
  NSString* preview_dir = @"/tmp/preview";
  
  render_envelope(file_path_cstr, [preview_dir UTF8String]);
  
  NSString* preview_path = get_x_path(preview_dir, kUTTypeImage);
  if (preview_path != nil) {

    printf("Generating image for %s\n", [preview_path UTF8String]);
    NSData* image_data = [[NSData alloc] initWithContentsOfFile:preview_path];
    QLPreviewRequestSetDataRepresentation(preview,(__bridge CFDataRef)image_data, kUTTypeImage, NULL);
    [[NSFileManager defaultManager] removeItemAtPath:@"/tmp/preview" error:nil];
    return noErr;
  }
  
  preview_path = get_x_path(preview_dir, kUTTypePDF);
  if (preview_path != nil) {
    printf("Generating pdf for %s\n", [preview_path UTF8String]);
    NSData* data = [[NSData alloc] initWithContentsOfFile:preview_path];    
    QLPreviewRequestSetDataRepresentation(preview,(__bridge CFDataRef)data, kUTTypePDF, NULL);
    [[NSFileManager defaultManager] removeItemAtPath:@"/tmp/preview" error:nil];
    return noErr;
  }
  preview_path = get_x_path(preview_dir, kUTTypeAudio);
  if (preview_path != nil) {
    printf("Generating audio for %s\n", [preview_path UTF8String]);
    NSData* data = [[NSData alloc] initWithContentsOfFile:preview_path];
    QLPreviewRequestSetDataRepresentation(preview,(__bridge CFDataRef)data, kUTTypeAudio, NULL);
    [[NSFileManager defaultManager] removeItemAtPath:@"/tmp/preview" error:nil];
    return noErr;
  }
  preview_path = get_x_path(preview_dir, kUTTypeMovie);
  if (preview_path != nil) {
    printf("Generating movie for %s\n", [preview_path UTF8String]);
    NSData* data = [[NSData alloc] initWithContentsOfFile:preview_path];
    QLPreviewRequestSetDataRepresentation(preview,(__bridge CFDataRef)data, kUTTypeMovie, NULL);
    [[NSFileManager defaultManager] removeItemAtPath:@"/tmp/preview" error:nil];
    return noErr;
  }

  [[NSFileManager defaultManager] removeItemAtPath:@"/tmp/preview" error:nil];
  return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
  // Implement only if supported
}
