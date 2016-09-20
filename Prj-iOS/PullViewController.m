//
//  PullViewController.m
//  AnyRtmp
//
//  Created by jianqiangzhang on 16/9/19.
//  Copyright © 2016年 EricTao. All rights reserved.
//

#import "PullViewController.h"
#import "RTMPGuestKit.h"

@interface PullViewController ()<RTMPGuestRtmpDelegate>
@property (nonatomic, strong) UIView *mainView;  // 主屏幕
@property (nonatomic, strong) UIButton *handupButton;
@property (nonatomic, strong) UIButton *closeButton;
@property (nonatomic, strong) UILabel *stateRTMPLabel;
@property (nonatomic, strong) RTMPGuestKit *guestKit;
@end

@implementation PullViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.navigationController setNavigationBarHidden:YES];
    self.view.backgroundColor = [UIColor whiteColor];
    
    [self.view addSubview:self.mainView];
    [self.view addSubview:self.handupButton];
    [self.view addSubview:self.closeButton];
    [self.view addSubview:self.stateRTMPLabel];
    
    self.guestKit = [[RTMPGuestKit alloc] initWithDelegate:self];
    if (self.urlStr) {
         [self.guestKit StartRtmpPlay:self.urlStr andRender:self.mainView];
    }
   
    
}
#pragma mark - button event -
- (void)closeButtonEvent:(UIButton*)sender {
    if (self.guestKit != nil) {
        [self.guestKit clear];
        self.guestKit = nil;
    }
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController popViewControllerAnimated:YES];
    });
}

#pragma mark -
- (void)OnRtmplayerOK {
    NSLog(@"OnRtmpStreamOK");
    self.stateRTMPLabel.text = @"连接RTMP服务成功";
}
- (void)OnRtmplayerStatus:(int) cacheTime withBitrate:(int) curBitrate {
    NSLog(@"OnRtmplayerStatus:%d withBitrate:%d",cacheTime,curBitrate);
    self.stateRTMPLabel.text = [NSString stringWithFormat:@"RTMP缓存区:%d 码率:%d",cacheTime,curBitrate];
}
- (void)OnRtmplayerCache:(int) time {
    NSLog(@"OnRtmplayerCache:%d",time);
    self.stateRTMPLabel.text = [NSString stringWithFormat:@"RTMP正在缓存:%d",time];
}

- (void)OnRtmplayerClosed:(int) errcode {
    
}
#pragma mark - get
- (UIView*)mainView {
    if (!_mainView) {
        _mainView = [[UIView alloc] init];
        _mainView.frame = CGRectMake(0, 0, CGRectGetWidth(self.view.frame), CGRectGetHeight(self.view.frame));
    }
    return _mainView;
}

- (UIButton*)closeButton {
    if(!_closeButton) {
        _closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_closeButton setImage:[UIImage imageNamed:@"close_preview"] forState:UIControlStateNormal];
        [_closeButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
        [_closeButton addTarget:self action:@selector(closeButtonEvent:) forControlEvents:UIControlEventTouchUpInside];
        _closeButton.frame = CGRectMake(CGRectGetMaxX(self.view.frame)-60, 20,40,40);
    }
    return _closeButton;
}
- (UILabel*)stateRTMPLabel {
    if (!_stateRTMPLabel) {
        _stateRTMPLabel = [UILabel new];
        _stateRTMPLabel.textColor = [UIColor redColor];
        _stateRTMPLabel.font = [UIFont systemFontOfSize:12];
        _stateRTMPLabel.textAlignment = NSTextAlignmentRight;
        _stateRTMPLabel.frame = CGRectMake(CGRectGetMaxX(self.view.frame)/2-10, CGRectGetMaxY(self.closeButton.frame), CGRectGetMaxX(self.view.frame)/2, 25);
        _stateRTMPLabel.text = @"未连接";
    }
    return _stateRTMPLabel;
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
