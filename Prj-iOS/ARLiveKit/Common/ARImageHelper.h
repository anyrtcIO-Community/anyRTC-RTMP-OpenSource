//
//  ARImageHelper.h
//  ARLiveKit
//
//  Created by 余生丶 on 2022/1/11.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface ARImageObject : NSObject
@property (nonatomic) unsigned char *data;
@property (nonatomic, assign) uint32_t width;
@property (nonatomic, assign) uint32_t height;
@end


@interface ARImageHelper : NSObject

/** Converts a UIImage to RGBA8 bitmap.
 @param image - a UIImage to be converted
 @return a RGBA8 bitmap, or NULL if any memory allocation issues. Cleanup memory with free() when done.
 */
+ (unsigned char *)convertUIImageToBitmapRGBA8:(UIImage *)image;

/** Converts a UIImage to RGBA8 bitmap.
 @param image - a UIImage to be converted
 @return ARImageObject
 */
+ (ARImageObject *)convertUIImageToARImageObject:(UIImage *)image;

/** A helper routine used to convert a RGBA8 to UIImage
 @return a new context that is owned by the caller
 */
+ (CGContextRef)newBitmapRGBA8ContextFromImage:(CGImageRef)image;


/** Converts a RGBA8 bitmap to a UIImage.
 @param buffer - the RGBA8 unsigned char * bitmap
 @param width - the number of pixels wide
 @param height - the number of pixels tall
 @return a UIImage that is autoreleased or nil if memory allocation issues
 */
+ (UIImage *)convertBitmapRGBA8ToUIImage:(unsigned char *)buffer
    withWidth:(int)width
    withHeight:(int)height;

@end

NS_ASSUME_NONNULL_END
