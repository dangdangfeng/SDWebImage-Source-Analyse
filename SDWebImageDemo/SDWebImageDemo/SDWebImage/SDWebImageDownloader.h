/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"
#import "SDWebImageOperation.h"

typedef NS_OPTIONS(NSUInteger, SDWebImageDownloaderOptions) {
    SDWebImageDownloaderLowPriority = 1 << 0,
    SDWebImageDownloaderProgressiveDownload = 1 << 1,

    /**
     * By default, request prevent the use of NSURLCache. With this flag, NSURLCache
     * is used with default policies.
     */
    SDWebImageDownloaderUseNSURLCache = 1 << 2, // 默认情况下请求不使用NSURLCache，如果设置该选项，则以默认的缓存策略来使用NSURLCache

    /**
     * Call completion block with nil image/imageData if the image was read from NSURLCache
     * (to be combined with `SDWebImageDownloaderUseNSURLCache`).
     */

    SDWebImageDownloaderIgnoreCachedResponse = 1 << 3,// 如果从NSURLCache缓存中读取图片，则使用nil作为参数来调用完成block
    /**
     * In iOS 4+, continue the download of the image if the app goes to background. This is achieved by asking the system for
     * extra time in background to let the request finish. If the background task expires the operation will be cancelled.
     */

    SDWebImageDownloaderContinueInBackground = 1 << 4,// 在iOS 4+系统上，允许程序进入后台后继续下载图片。该操作通过向系统申请额外的时间来完成后台下载。如果后台任务终止，则操作会被取消

    /**
     * Handles cookies stored in NSHTTPCookieStore by setting 
     * NSMutableURLRequest.HTTPShouldHandleCookies = YES;
     */
    SDWebImageDownloaderHandleCookies = 1 << 5,// 通过设置NSMutableURLRequest.HTTPShouldHandleCookies = YES来处理存储在NSHTTPCookieStore中的cookie

    /**
     * Enable to allow untrusted SSL certificates.
     * Useful for testing purposes. Use with caution in production.
     */
    SDWebImageDownloaderAllowInvalidSSLCertificates = 1 << 6,// 允许不受信任的SSL证书。主要用于测试目的

    /**
     * Put the image in the high priority queue.
     */
    SDWebImageDownloaderHighPriority = 1 << 7,// 将图片下载放到高优先级队列中
};

typedef NS_ENUM(NSInteger, SDWebImageDownloaderExecutionOrder) {
    /**
     * Default value. All download operations will execute in queue style (first-in-first-out).
     */
    SDWebImageDownloaderFIFOExecutionOrder,// 以队列的方式，按照先进先出的顺序下载。这是默认的下载顺序

    /**
     * All download operations will execute in stack style (last-in-first-out).
     */
    SDWebImageDownloaderLIFOExecutionOrder// 以栈的方式，按照后进先出的顺序下载。
};

extern NSString *const SDWebImageDownloadStartNotification;
extern NSString *const SDWebImageDownloadStopNotification;
// 下载进度
typedef void(^SDWebImageDownloaderProgressBlock)(NSInteger receivedSize, NSInteger expectedSize);
// 下载完成
typedef void(^SDWebImageDownloaderCompletedBlock)(UIImage *image, NSData *data, NSError *error, BOOL finished);
// Header过滤
typedef NSDictionary *(^SDWebImageDownloaderHeadersFilterBlock)(NSURL *url, NSDictionary *headers);

/**
 * Asynchronous downloader dedicated and optimized for image loading.
 */
@interface SDWebImageDownloader : NSObject

/**
 * Decompressing images that are downloaded and cached can improve performance but can consume lot of memory.
 * Defaults to YES. Set this to NO if you are experiencing a crash due to excessive memory consumption.
 */
@property (assign, nonatomic) BOOL shouldDecompressImages;//解压下载和缓存的图像可以提高性能,但会消耗很多内存。默认值为YES。如果由于过度消耗内存而崩溃，将这个参数设置为NO

@property (assign, nonatomic) NSInteger maxConcurrentDownloads;//默认情况下，队列最大并发数是6

/**
 * Shows the current amount of downloads that still need to be downloaded
 */
@property (readonly, nonatomic) NSUInteger currentDownloadCount;//需要被下载的数量


/**
 *  The timeout value (in seconds) for the download operation. Default: 15.0.
 */
@property (assign, nonatomic) NSTimeInterval downloadTimeout;// 下载超时，默认15s


/**
 * Changes download operations execution order. Default value is `SDWebImageDownloaderFIFOExecutionOrder`.
 */
@property (assign, nonatomic) SDWebImageDownloaderExecutionOrder executionOrder;//改变下载模式

/**
 *  Singleton method, returns the shared instance
 *
 *  @return global shared instance of downloader class
 */
+ (SDWebImageDownloader *)sharedDownloader;//单例

/**
 *  Set the default URL credential to be set for request operations.
 */
@property (strong, nonatomic) NSURLCredential *urlCredential;//默认URL凭证

/**
 * Set username
 */
@property (strong, nonatomic) NSString *username;

/**
 * Set password
 */
@property (strong, nonatomic) NSString *password;

/**
 * Set filter to pick headers for downloading image HTTP request.
 *
 * This block will be invoked for each downloading image request, returned
 * NSDictionary will be used as headers in corresponding HTTP request.
 */
@property (nonatomic, copy) SDWebImageDownloaderHeadersFilterBlock headersFilter;//下载请求头过滤器

/**
 * Set a value for a HTTP header to be appended to each download HTTP request.
 *
 * @param value The value for the header field. Use `nil` value to remove the header.
 * @param field The name of the header field to set.
 */
- (void)setValue:(NSString *)value forHTTPHeaderField:(NSString *)field;//设置请求头

/**
 * Returns the value of the specified HTTP header field.
 *
 * @return The value associated with the header field field, or `nil` if there is no corresponding header field.
 */
- (NSString *)valueForHTTPHeaderField:(NSString *)field;//请求头

/**
 * Sets a subclass of `SDWebImageDownloaderOperation` as the default
 * `NSOperation` to be used each time SDWebImage constructs a request
 * operation to download an image.
 *
 * @param operationClass The subclass of `SDWebImageDownloaderOperation` to set 
 *        as default. Passing `nil` will revert to `SDWebImageDownloaderOperation`.
 */
- (void)setOperationClass:(Class)operationClass;//为NSOperation设置默认子类，供下载使用

/**
 整个下载管理器对于下载请求的管理
 * Creates a SDWebImageDownloader async downloader instance with a given URL
 *
 * The delegate will be informed when the image is finish downloaded or an error has happen.
 *
 * @see SDWebImageDownloaderDelegate
 *
 * @param url            The URL to the image to download
 * @param options        The options to be used for this download
 * @param progressBlock  A block called repeatedly while the image is downloading
 * @param completedBlock A block called once the download is completed.
 *                       If the download succeeded, the image parameter is set, in case of error,
 *                       error parameter is set with the error. The last parameter is always YES
 *                       if SDWebImageDownloaderProgressiveDownload isn't use. With the
 *                       SDWebImageDownloaderProgressiveDownload option, this block is called
 *                       repeatedly with the partial image object and the finished argument set to NO
 *                       before to be called a last time with the full image and finished argument
 *                       set to YES. In case of error, the finished argument is always YES.
 *
 * @return A cancellable SDWebImageOperation
 */
- (id <SDWebImageOperation>)downloadImageWithURL:(NSURL *)url
                                         options:(SDWebImageDownloaderOptions)options
                                        progress:(SDWebImageDownloaderProgressBlock)progressBlock
                                       completed:(SDWebImageDownloaderCompletedBlock)completedBlock;

/**
 * Sets the download queue suspension state
 */
- (void)setSuspended:(BOOL)suspended;//下载队列暂停

/**
 * Cancels all download operations in the queue
 */
- (void)cancelAllDownloads;//取消所有下载

@end
