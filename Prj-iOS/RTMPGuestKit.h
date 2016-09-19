/**
*  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
*
*  Please visit https://www.anyrtc.io for detail.
*
* The GNU General Public License is a free, copyleft license for
* software and other kinds of works.
*
* The licenses for most software and other practical works are designed
* to take away your freedom to share and change the works.  By contrast,
* the GNU General Public License is intended to guarantee your freedom to
* share and change all versions of a program--to make sure it remains free
* software for all its users.  We, the Free Software Foundation, use the
* GNU General Public License for most of our software; it applies also to
* any other work released this way by its authors.  You can apply it to
* your programs, too.
* See the GNU LICENSE file for more info.
*/

#ifndef RTMPGuestKit_h
#define RTMPGuestKit_h
#import <UIKit/UIKit.h>
#import "RTMPGuestDelegate.h"

typedef NS_ENUM(NSInteger,VideoShowMode){
    VideoShowModeScaleAspectFit,   
    VideoShowModeScaleAspectFill,  // default by height scale (高度填充整个屏幕)
    VideoShowModeCenter
};

@interface RTMPGuestKit : NSObject {
    
}
/**
 *  Player show mode
 */
@property (nonatomic, assign)VideoShowMode videoContentMode;
/**
 *  Initialize the guest clent
 *
 *  @param delegate RTMPCGuestRtmpDelegate
 *
 *  @return guest object
 */
- (instancetype)initWithDelegate:(id<RTMPGuestRtmpDelegate>)delegate;
/**
 *  clear guest clent (if you leave,you must call this function)
 */
- (void) clear;

#pragma mark Rtmp function for pull rtmp stream
/**
 *  start play the video
 *
 *  @param strUrl rtmp address
 *  @param render video view
 *
 *  @return scuess or failed
 */
- (BOOL)StartRtmpPlay:(NSString*)strUrl andRender:(UIView*)render;
/**
 *  stop play
 */
- (void)StopRtmpPlay;
@end

#endif /* RTMPGuestKit_h */
