//
//  ViewController.m
//  HelloWorld
//
//  Created by whoosh-build on 27/10/2023.
//

#include "ViewController.h"
#include "Application/AppConfig.h"
#include "Core/LCUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>


std::string ReadTextFile2(const char* filePath)
{
    using namespace std::filesystem;

    std::string result;
    FILE *dic;
    dic = fopen("/Users/whoosh-build/Documents/UE427Source/README.md", "rb");
    fclose(dic);

    path path;
    path.assign(filePath);
    std::ifstream stream("/Users/whoosh-build/Documents/UE427Source/README.md", std::ios::in);
    auto op = stream.is_open();

    const auto sz = file_size(path);
    result = std::string(sz, '\0');
    stream.read(&result[0], sz);

    return result;
}

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    try
    {
        // Set scheme Working directory to ${SRCROOT}
        NSString* srcRoot = [[NSProcessInfo processInfo] environment][@"PWD"];
        NSString* cfgPath = [srcRoot stringByAppendingString: @"/../../Assets/config.txt"];

        LcAppConfig cfg;
        const char* pat = [cfgPath UTF8String];
        std::string rr = ReadTextFile2(pat);
        LoadConfig(cfg, pat);
    }
    catch (const std::exception& ex)
    {
        DebugMsg(ex.what());
    }

    NSLog(@"Frame %.0f, %.0f", self.view.frame.size.width, self.view.frame.size.height);
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
}

@end
