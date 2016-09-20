//
//  PushViewController.m
//  AnyRtmp
//
//  Created by jianqiangzhang on 16/9/19.
//  Copyright © 2016年 EricTao. All rights reserved.
//

#import "PushViewController.h"
#import "RTMPHosterKit.h"

@interface PushViewController ()<RTMPHosterRtmpDelegate>
@property (nonatomic, strong) RTMPHosterKit *hosterKit;

@property (nonatomic, strong) UIView *cameraView;  // 推流
@property (nonatomic, strong) UIButton *closeButton;

@property (nonatomic, strong) UILabel *stateRTMPLabel;
@property (nonatomic, strong) UIButton *cameraButton;
@property (nonatomic, strong) UIButton *beautyButton;
@end

@implementation PushViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [super viewDidLoad];
    [self.navigationController setNavigationBarHidden:YES];
    self.view.backgroundColor = [UIColor whiteColor];
    
    [self.view addSubview:self.cameraView];
    [self.view addSubview:self.closeButton];
    [self.view addSubview:self.stateRTMPLabel];
    [self.view addSubview:self.cameraButton];
    [self.view addSubview:self.beautyButton];
    
    self.hosterKit = [[RTMPHosterKit alloc] initWithDelegate:self];
    [self.hosterKit SetVideoMode:RTMP_Video_SD];
    [self.hosterKit SetVideoCapturer:self.cameraView andUseFront:YES];
    if (self.urlStr) {
         [self.hosterKit StartPushRtmpStream:self.urlStr];
    }
   
    
}
#pragma mark - button event
- (void)closeButtonEvent:(UIButton*)sender {
    if (self.hosterKit) {
        [self.hosterKit StopRtmpStream];
        [self.hosterKit clear];
        self.hosterKit = nil;
    }
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.navigationController popViewControllerAnimated:YES];
    });
}
- (void)cameraButtonEvent:(UIButton*)sender {
    if (self.hosterKit) {
        [self.hosterKit SwitchCamera];
    }
}
- (void)beautyButtonEvent:(UIButton*)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self.hosterKit SetBeautyEnable:NO];
    }else{
        [self.hosterKit SetBeautyEnable:YES];
    }
}

#pragma mark -  RTMPHosterRtmpDelegate
- (void)OnRtmpStreamOK {
    NSLog(@"OnRtmpStreamOK");
    self.stateRTMPLabel.text = @"连接RTMP服务成功";
}

- (void)OnRtmpStreamReconnecting:(int) times {
    NSLog(@"OnRtmpStreamReconnecting:%d",times);
    self.stateRTMPLabel.text = [NSString stringWithFormat:@"第%d次重连中...",times];
}

- (void)OnRtmpStreamStatus:(int) delayMs withNetBand:(int) netBand {
    NSLog(@"OnRtmpStreamStatus:%d withNetBand:%d",delayMs,netBand);
    self.stateRTMPLabel.text = [NSString stringWithFormat:@"RTMP延迟:%d 网络:%d",delayMs,netBand];
}

- (void)OnRtmpStreamFailed:(int) code {
    NSLog(@"OnRtmpStreamFailed:%d",code);
    self.stateRTMPLabel.text = @"连接RTMP服务失败";
}

- (void)OnRtmpStreamClosed {
    NSLog(@"OnRtmpStreamClosed");
}

#pragma mark - get
- (UIView*)cameraView {
    if (!_cameraView) {
        _cameraView = [[UIView alloc] init];
        _cameraView.frame = CGRectMake(0, 0, CGRectGetWidth(self.view.frame), CGRectGetHeight(self.view.frame));
        _cameraView.layer.borderColor = [UIColor grayColor].CGColor;
        _cameraView.layer.borderWidth = .5;
    }
    return _cameraView;
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
- (UIButton*)cameraButton {
    if (!_cameraButton) {
        _cameraButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_cameraButton setImage:[UIImage imageNamed:@"camra_preview"] forState:UIControlStateNormal];
        [_cameraButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
        [_cameraButton addTarget:self action:@selector(cameraButtonEvent:) forControlEvents:UIControlEventTouchUpInside];
        _cameraButton.frame = CGRectMake(CGRectGetMinX(self.closeButton.frame)-60, 20,40,40);
    }
    return _cameraButton;
}
- (UIButton*)beautyButton {
    if (!_beautyButton) {
        _beautyButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_beautyButton setImage:[UIImage imageNamed:@"camra_beauty"] forState:UIControlStateNormal];
        [_beautyButton setImage:[UIImage imageNamed:@"camra_beauty_close"] forState:UIControlStateSelected];
        [_beautyButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
        [_beautyButton addTarget:self action:@selector(beautyButtonEvent:) forControlEvents:UIControlEventTouchUpInside];
        _beautyButton.frame = CGRectMake(CGRectGetMinX(self.cameraButton.frame)-60, 20,40,40);
    }
    return _beautyButton;
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
