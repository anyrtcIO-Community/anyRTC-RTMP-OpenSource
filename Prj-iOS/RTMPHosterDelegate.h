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

#ifndef RTMPHosterDelegate_h
#define RTMPHosterDelegate_h

//* All callback event is on main thread, so it's mean developer
//* could operate UI method in callback directly.
@protocol RTMPHosterRtmpDelegate <NSObject>
@required
/**
 *   RTMP service connection is successful callback
 */
- (void)OnRtmpStreamOK;
/**
 *  RTMP service reconnection callback
 *
 *  @param times reconnection times
 */
- (void)OnRtmpStreamReconnecting:(int) times;
/**
 *  RTMP's delay time and the current network bandwidth
 *
 *  @param delayMs delay time (ms)
 *  @param netBand network bandwidth
 */
- (void)OnRtmpStreamStatus:(int) delayMs withNetBand:(int) netBand;
/**
 *  RTMP service connection is failure callback
 *
 *  @param code why
 */
- (void)OnRtmpStreamFailed:(int) code;
/**
 *  RTMP server close callback
 */
- (void)OnRtmpStreamClosed;
@end

#endif /* RTMPHosterDelegate_h */
