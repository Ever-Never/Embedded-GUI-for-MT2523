//
//  SmartConnectionTableViewController.m
//  SmartConnection
//
//  Created by user on 2017/2/22.
//  Copyright © 2017年 Mediatek. All rights reserved.
//

#import "SmartConnectionTableViewController.h"
#import <SystemConfiguration/CaptiveNetwork.h>
#import <CoreFoundation/CoreFoundation.h>
#import "elian.h"

@interface SmartConnectionTableViewController ()
@property (weak, nonatomic) IBOutlet UITextField *wifiApName;
@property (weak, nonatomic) IBOutlet UITextField *wifiApPassword;
@property (weak, nonatomic) IBOutlet UITextField *customInfo;
@property (weak, nonatomic) IBOutlet UISwitch *versionV1;
@property (weak, nonatomic) IBOutlet UISwitch *versionV4;
@property (weak, nonatomic) IBOutlet UISwitch *versionV5;
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UIButton *stopButton;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *runningIndicator;
@property (weak, nonatomic) IBOutlet UITextField *oldversioninterval;
@property (weak, nonatomic) IBOutlet UITextField *nversionInterval;
@property (weak, nonatomic) IBOutlet UITextField *etEncryptionKey;

@end

void *context;

@implementation SmartConnectionTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    _wifiApName.delegate = self;
    _wifiApPassword.delegate = self;
    _customInfo.delegate = self;
    [self initUI];
    
    [_wifiApName addTarget:self action:@selector(textFieldChanged:) forControlEvents:UIControlEventEditingChanged];
    [_wifiApPassword addTarget:self action:@selector(textFieldChanged:) forControlEvents:UIControlEventEditingChanged];
    [_customInfo addTarget:self action:@selector(textFieldChanged:) forControlEvents:UIControlEventEditingChanged];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)updateName
{
    CFArrayRef array = CNCopySupportedInterfaces();
    if (array != nil) {
        CFDictionaryRef dic = CNCopyCurrentNetworkInfo((CFStringRef)CFArrayGetValueAtIndex(array, 0));
        if (dic != nil) {
            NSDictionary *dd = (NSDictionary *)CFBridgingRelease(dic);
            NSString *name = [dd valueForKey:@"SSID"];
            [_wifiApName setText:name];
            NSLog(@"surplus :: Set name %@", [_wifiApName text]);
        } else {
            NSLog(@"surplus :: bridging release dictionary is nil");
        }
    } else {
        NSLog(@"surplus supported interfaces is nil");
    }
}

-(void)viewDidAppear:(BOOL)animated {
    [self updateName];
}

-(void)dismissKeyboard {
    [_wifiApName resignFirstResponder];
    [_wifiApPassword resignFirstResponder];
    [_customInfo resignFirstResponder];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 6;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    NSInteger retValue = 0;
    switch (section) {
        case 0:
            retValue = 2;
            break;
            
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            retValue = 1;
            break;
            
        default:
            retValue = 0;
            break;
    }
    return retValue;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:NO];
}

#pragma mark - Watch the text field change listener

-(void)textFieldChanged:(UITextField *)textField {
    const char *cc;
    BOOL clear = NO;
    if (textField == _wifiApName) {
        cc = [[_wifiApName text] UTF8String];
        if (strlen(cc) > 32) {
            [self updateError:@"SSID" error:ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH];
        } else {
            clear = YES;
        }
    } else if (textField == _wifiApPassword) {
        cc = [[_wifiApPassword text] UTF8String];
        if (strlen(cc) > 64) {
            [self updateError:@"Password" error:ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH];
        } else {
            clear = YES;
        }
    } else if (textField == _customInfo) {
        cc = [[_customInfo text] UTF8String];
        if (strlen(cc) > 640) {
            [self updateError:@"CusInfo" error:ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH];
        } else {
            clear = YES;
        }
    }
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField {
    [self dismissKeyboard];
    return NO;
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self dismissKeyboard];
}
#pragma mark - initui

-(void)initUI {
    [_runningIndicator setHidesWhenStopped:YES];
    [_runningIndicator stopAnimating];
    [_versionV1 setOn:NO];
    [_versionV4 setOn:NO];
    [_versionV5 setOn:NO];
    [_startButton setEnabled:YES];
    [_stopButton setEnabled:NO];
    [_nversionInterval setEnabled:YES];
    [_oldversioninterval setEnabled:YES];
    
    int proV = 0;
    int libV = 0;
    elianGetVersion(&proV, &libV);
    self.title = [NSString stringWithFormat:@"Smart Connection (V%d.%d)", proV, libV];
}

#pragma mark - show warning dialog
-(void)showWarningAlertDialog:(NSString*)warningMessage {
    if([warningMessage isEqualToString:@""] == YES) {
        NSLog(@"showWarningAlertDialog :: No Message To Show");
        return;
    }
    
    @autoreleasepool {
        dispatch_async(dispatch_get_main_queue(), ^() {
            UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Warning"
                                                                                message:warningMessage
                                                                         preferredStyle:UIAlertControllerStyleAlert];
            UIAlertAction *action = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                [controller dismissViewControllerAnimated:YES completion:nil];
            }];
            [controller addAction:action];
            [self presentViewController:controller animated:YES completion:nil];
        });
    }
}

-(NSString *)errorString:(int)error {
    NSString *str;
    switch(error) {
        case ELIAN_ERROR_CODE_NOT_INITED:
            str = @"Not inited";
            break;
        case ELIAN_ERROR_CODE_WRONG_TYPE:
            str = @"Wong type";
            break;
        case ELIAN_ERROR_CODE_WRONG_PARAMETER:
            str = @"Wrong Parameter";
            break;
        case ELIAN_ERROR_CODE_ALREADY_STOPED:
            str = @"Already Stopped";
            break;
        case ELIAN_ERROR_CODE_ALREADY_STARTED:
            str = @"Already Started";
            break;
        case ELIAN_ERROR_CODE_CRYPTED_FAILED:
            str = @"Crypted Failed";
            break;
        case ELIAN_ERROR_CODE_FAILED_TO_START:
            str = @"Failed to start";
            break;
        case ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH:
            str = @"Exceed Max Length";
            break;
        case ELIAN_ERROR_CODE_NOT_ENOUGH_MEMORY:
            str = @"Not Enough Memory";
            break;
        default:
            str = @"Unknown Error";
            break;
    }
    return str;
}

-(void)updateError:(NSString *)header error:(int)err {
    
    NSString *e = nil;
    NSString *ss = nil;
    
    if (err == 0xff) {
        ss = [NSString stringWithFormat:@"%@", header];
    } else {
        e = [self errorString:err];
        ss = [NSString stringWithFormat:@"%@ With Error <%@>", header, e];
    }
   
    [self showWarningAlertDialog:ss];
}

#pragma mark - start button handle action
- (IBAction)startButtonAction:(id)sender {
    
    if ([_versionV5 isOn] == NO
        && [_versionV1 isOn] == NO
        && [_versionV4 isOn] == NO) {
        [self updateError:@"Please Select At Least 1 Version To Send" error:0xff];
        return;
    }
    
    const char *ssid = NULL;//[[_wifiApName text] UTF8String];
    const char *pwd = NULL;//[[_wifiApPassword text] UTF8String];
    const char *cinfo = NULL;
    const char *encry_key = NULL;
    
    unsigned char target[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    float oi = 0.0f;
    float ni = 0.0f;
    int retValue = ELIAN_ERROR_CODE_OK;
    int flag = 0;
    
    NSLog(@"SSID                : %@", [_wifiApName text]);
    NSLog(@"Password            : %@", [_wifiApPassword text]);
    NSLog(@"Customer Info       : %@", [_customInfo text]);
    NSLog(@"Encryption Key      : %@", [_etEncryptionKey text]);
    
    if ([_versionV1 isOn] == YES) {
        flag |= ELIAN_VERSION_V1;
    }
    if ([_versionV4 isOn] == YES) {
        flag |= ELIAN_VERSION_V4;
    }
    if ([_versionV5 isOn] == YES) {
        flag |= ELIAN_VERSION_V5;
    }
    
    if ([_etEncryptionKey isEqual:@""] == YES) {
        context = elianNew(NULL, 0, target, flag);
    } else {
        encry_key = _etEncryptionKey.text.UTF8String;
        context = elianNew((char *)encry_key, (int)strlen(encry_key), target, flag);
    }
    
    if (context == NULL) {
        [self updateError:@"Failed To Create Context" error:0xff];
        return;
    }
    if ([[_wifiApName text]isEqualToString:@""] == NO) {
        ssid = [[_wifiApName text] UTF8String];
        retValue = elianPut(context, TYPE_ID_SSID, (char *)ssid, (int)strlen(ssid));
    }
    if (retValue != ELIAN_ERROR_CODE_OK) {
        [self updateError:@"Failed To Set SSID" error:retValue];
        elianDestroy(context);
        return;
    }
    if ([[_wifiApPassword text]isEqualToString:@""] == NO) {
        pwd = [[_wifiApPassword text] UTF8String];
        retValue = elianPut(context, TYPE_ID_PWD, (char *)pwd, (int)strlen(pwd));
    }
    if (retValue != ELIAN_ERROR_CODE_OK) {
        [self updateError:@"Failed To Set Password" error:retValue];
        elianDestroy(context);
        return;
    }
    if ([[_customInfo text]isEqualToString:@""] == NO) {
        cinfo = [[_customInfo text] UTF8String];
        retValue = elianPut(context, TYPE_ID_CUST, (char *)cinfo, (int)strlen(cinfo));
    }
    if (retValue != ELIAN_ERROR_CODE_OK) {
        [self updateError:@"Failed To Set Customer Info" error:retValue];
        elianDestroy(context);
        return;
    }
    
    if ([[_oldversioninterval text] isEqualToString:@""] == NO) {
        oi = [[_oldversioninterval text] floatValue];
    }
    if ([[_nversionInterval text] isEqualToString:@""] == NO) {
        ni = [[_nversionInterval text] floatValue];
    }
    if (oi != 0.0 || ni != 0.0) {
        retValue = elianSetInterval(context, oi * 1000, ni * 1000);
    }
    
    if (retValue != ELIAN_ERROR_CODE_OK) {
        [self updateError:@"Failed To Set Sending Interval" error:retValue];
        elianDestroy(context);
        return;
    }

    retValue = elianStart(context);
    if (retValue != ELIAN_ERROR_CODE_OK) {
        [self updateError:@"Failed To Start Smart Connection" error:retValue];
        return;
    }
    
    [_startButton setEnabled:NO];
    [_stopButton setEnabled:YES];
    [_runningIndicator startAnimating];
    [_wifiApName setEnabled:NO];
    [_wifiApPassword setEnabled:NO];
    [_customInfo setEnabled:NO];
    [_versionV4 setEnabled:NO];
    [_versionV1 setEnabled:NO];
    [_versionV5 setEnabled:NO];
    [_nversionInterval setEnabled:NO];
    [_oldversioninterval setEnabled:NO];
    [_etEncryptionKey setEnabled:NO];
    
}

#pragma mark - stop button handle action
- (IBAction)stopButtonAction:(id)sender {
    
    elianStop(context);
    elianDestroy(context);
    
    [_startButton setEnabled:YES];
    [_stopButton setEnabled:NO];
    [_runningIndicator stopAnimating];
    [_wifiApName setEnabled:YES];
    [_wifiApPassword setEnabled:YES];
    [_customInfo setEnabled:YES];
    [_versionV4 setEnabled:YES];
    [_versionV1 setEnabled:YES];
    [_versionV5 setEnabled:YES];
    [_nversionInterval setEnabled:YES];
    [_oldversioninterval setEnabled:YES];
    [_etEncryptionKey setEnabled:YES];
}



@end
