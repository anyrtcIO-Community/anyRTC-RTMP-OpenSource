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

#ifndef RTMPHosterKit_h
#define RTMPHosterKit_h
#import <UIKit/UIKit.h>
#import "RTMPHosterDelegate.h"
#import "RTMPCommon.h"

@interface RTMPHosterKit : NSObject {
    
}

- (instancetype)initWithDelegate:(id<RTMPHosterRtmpDelegate>)delegate;
/**
 *  clear host clent (if you leave,you must call this function)
 */
- (void) clear;

#pragma mark Common function
/**
 *  audio setting (default is YES)
 *
 *  @param enable set YES to enable, NO to disable.
 */
- (void)SetAudioEnable:(bool) enabled;
/**
 *  video setting (default is YES)
 *
 *  @param enable set YES to enable, NO to disable.
 */
- (void)SetVideoEnable:(bool) enabled;
/**
 *  video view show and setting
 *
 *  @param render video view
 *  @param front  camera front or back  if yes front or back
 */
- (void)SetVideoCapturer:(UIView*) render andUseFront:(bool)front;
/**
 *  change camera front or back
 */
- (void)SwitchCamera;
/**
 *  Set the beauty
 *
 *  @param enabled YES/NO:beauty or normal
 */
- (void)SetBeautyEnable:(bool) enabled;
/**
 *  The video quality setting
 *
 *  @param videoMode quality type
 */
- (void)SetVideoMode:(RTMPVideoMode) videoMode;

#pragma mark Rtmp function for push rtmp stream
/**
 *  start push stream server
 *
 *  @param strUrl server address
 *
 *  @return scuess or failed
 */
- (BOOL)StartPushRtmpStream:(NSString*)strUrl;
/**
 *  stop push stream to the server
 */
- (void)StopRtmpStream;
@end

#endif /* RTMPHosterKit_h */
