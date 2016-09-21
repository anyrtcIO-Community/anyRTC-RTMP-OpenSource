//
//  ViewController.m
//  AnyRtmp
//
//  Created by EricTao on 16/9/16.
//  Copyright © 2016年 EricTao. All rights reserved.
//

#import "ViewController.h"
#import "PushViewController.h"
#import "PullViewController.h"

@interface ViewController ()
@property (nonatomic, strong) UIImageView *headImageView;
@property (nonatomic, strong) UITextField *textField;
@property (nonatomic, strong) UIButton *pushButton;
@property (nonatomic, strong) UIButton *pullButton;
@property (nonatomic, strong) UILabel *powerLabel;

@end

@implementation ViewController
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view addSubview:self.headImageView];
    [self.view addSubview:self.textField];
    [self.view addSubview:self.pushButton];
    [self.view addSubview:self.pullButton];
    [self.view addSubview:self.powerLabel];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapEvent:)];
    [self.view addGestureRecognizer:tap];
}

#pragma mark - button event 
- (void)tapEvent:(UITapGestureRecognizer*)gesture {
    if (self.textField.isEditing) {
        [self.textField resignFirstResponder];
    }
}
- (void)pushButtonEvent:(UIButton*)sender {
    PushViewController *push = [PushViewController new];
    push.urlStr = self.textField.text;
    [self.navigationController pushViewController:push animated:YES];
}
- (void)pullButtonEvent:(UIButton*)sender {
    PullViewController *pull  = [PullViewController new];
    pull.urlStr = self.textField.text;
    [self.navigationController pushViewController:pull animated:YES];
}

#pragma mark - get
- (UIImageView *)headImageView {
    if (!_headImageView) {
        _headImageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 200, 70)];
        _headImageView.image = [UIImage imageNamed:@"anyrtc"];
        _headImageView.center = CGPointMake(self.view.center.x, self.view.center.y-200);
    }
    return _headImageView;
}
- (UITextField*)textField {
    if (!_textField) {
        _textField = [[UITextField alloc] initWithFrame:CGRectMake(20, CGRectGetMaxY(self.headImageView.frame)+20, CGRectGetWidth(self.view.frame)-40, 45)];
        _textField.placeholder = @"请输入URL";
        _textField.borderStyle = UITextBorderStyleLine;
    }
    return _textField;
}
- (UIButton*)pushButton {
    if (!_pushButton) {
        _pushButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_pushButton addTarget:self action:@selector(pushButtonEvent:) forControlEvents:UIControlEventTouchUpInside];
        _pushButton.frame = CGRectMake(20, CGRectGetMaxY(self.textField.frame)+20, (CGRectGetWidth(self.view.frame)- 60)/2, 45);
        [_pushButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        _pushButton.layer.borderColor = [UIColor blackColor].CGColor;
        _pushButton.layer.borderWidth = .5;
        [_pushButton setTitle:@"推流" forState:UIControlStateNormal];
    }
    return _pushButton;
}
- (UIButton*)pullButton {
    if (!_pullButton) {
        _pullButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [_pullButton addTarget:self action:@selector(pullButtonEvent:) forControlEvents:UIControlEventTouchUpInside];
        _pullButton.frame = CGRectMake(CGRectGetMaxX(self.pushButton.frame)+20, CGRectGetMaxY(self.textField.frame)+20, (CGRectGetWidth(self.view.frame)- 60)/2, 45);
        [_pullButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        _pullButton.layer.borderColor = [UIColor blackColor].CGColor;
        _pullButton.layer.borderWidth = .5;
        [_pullButton setTitle:@"拉流" forState:UIControlStateNormal];
    }
    return _pullButton;
}
- (UILabel*)powerLabel {
    if (!_powerLabel) {
        _powerLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, CGRectGetHeight(self.view.frame)- 40, CGRectGetWidth(self.view.frame), 35)];
        _powerLabel.text = @"Powered by Shanghai Boyuan(DYNC) Information Technology CO...LTD";
        _powerLabel.numberOfLines = 0;
        _powerLabel.textAlignment = NSTextAlignmentCenter;
        _powerLabel.font = [UIFont systemFontOfSize:12];
        _powerLabel.textColor = [UIColor blackColor];
    }
    return _powerLabel;
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
