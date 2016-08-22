/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "SDWebImageCompat.h"
#import "SDWebImageManager.h"

/**
 * Integrates SDWebImage async downloading and caching of remote images with UIImageView.
 *
 * Usage with a UITableViewCell sub-class:
 *
 * @code

#import <SDWebImage/UIImageView+WebCache.h>

...

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *MyIdentifier = @"MyIdentifier";
 
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];
 
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MyIdentifier]
                 autorelease];
    }
 
    // Here we use the provided sd_setImageWithURL: method to load the web image
    // Ensure you use a placeholder image otherwise cells will be initialized with no image
    [cell.imageView sd_setImageWithURL:[NSURL URLWithString:@"http://example.com/image.jpg"]
                      placeholderImage:[UIImage imageNamed:@"placeholder"]];
 
    cell.textLabel.text = @"My Text";
    return cell;
}

 * @endcode
 */
@interface UIImageView (WebCache)

/**
 * Get the current image URL.
 *
 * Note that because of the limitations of categories this property can get out of sync
 * if you use sd_setImage: directly.
 */
- (NSURL *)sd_imageURL;//得到当前图片的url

/**
 * Set the imageView `image` with an `url`.
 *
 * The download is asynchronous and cached.
 *
 * @param url The url for the image.
 */
- (void)sd_setImageWithURL:(NSURL *)url;//异步下载图片并缓存

/**
 * Set the imageView `image` with an `url` and a placeholder.
 *
 * The download is asynchronous and cached.
 *
 * @param url         The url for the image.
 * @param placeholder The image to be set initially, until the image request finishes.
 * @see sd_setImageWithURL:placeholderImage:options:
 */
- (void)sd_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder;//异步下载图片并缓存，没下载完之前先显示占位图片，下载完之后再替换

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 *
 * The download is asynchronous and cached.
 *
 * @param url         The url for the image.(下载图片路径)
 * @param placeholder The image to be set initially, until the image request finishes.(占位图片，直到下载完成才替换)
 * @param options     The options to use when downloading the image. @see SDWebImageOptions for the possible values.(下载图片选择方式)
 */
- (void)sd_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options;//异步下载图片并缓存

/**
 * Set the imageView `image` with an `url`.
 * 异步下载图片并缓存，完成后可以在block中做事情
 * The download is asynchronous and cached.
 *
 * @param url            The url for the image.
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 */
- (void)sd_setImageWithURL:(NSURL *)url completed:(SDWebImageCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder.
 *
 * The download is asynchronous and cached.
 *
 * @param url            The url for the image.(下载图片url)
 * @param placeholder    The image to be set initially, until the image request finishes.
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.(block中可以得到下载图片，错误信息，缓存类型，下载图片地址  参数，给用户做相应操作)
 */
- (void)sd_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder completed:(SDWebImageCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 * 异步下载图片并缓存，提供占位图片，并完成后可以在block中做事情
 * The download is asynchronous and cached.
 *
 * @param url            The url for the image.(下载图片url)
 * @param placeholder    The image to be set initially, until the image request finishes.(占位图片，直到下载完成才替换)
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.(下载图片选择方式)
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.( block中可以得到下载图片，错误信息，缓存类型，下载图片地址  参数，给用户做相应操作)
 */
- (void)sd_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options completed:(SDWebImageCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 * 异步下载图片并缓存，可以监听下载进度，完成后可以在block中做事情
 * The download is asynchronous and cached.
 *
 * @param url            The url for the image.
 * @param placeholder    The image to be set initially, until the image request finishes.
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * @param progressBlock  A block called while image is downloading(SDWebImageDownloaderProgressBlock:NSInteger receivedSize(当前下载大小), NSInteger expectedSize(总大小))
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 */
- (void)sd_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options progress:(SDWebImageDownloaderProgressBlock)progressBlock completed:(SDWebImageCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url` and optionally a placeholder image.
 * 异步下载图片并缓存，可以监听下载进度，完成后可以在block中做事情
 * The download is asynchronous and cached.
 *
 * @param url            The url for the image.
 * @param placeholder    The image to be set initially, until the image request finishes.
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * @param progressBlock  A block called while image is downloading
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 */
- (void)sd_setImageWithPreviousCachedImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options progress:(SDWebImageDownloaderProgressBlock)progressBlock completed:(SDWebImageCompletionBlock)completedBlock;

/**
 * Download an array of images and starts them in an animation loop
 * 动画效果
 * @param arrayOfURLs An array of NSURL
 */
- (void)sd_setAnimationImagesWithURLs:(NSArray *)arrayOfURLs;

/**
 * Cancel the current download（取消当前下载）
 */
- (void)sd_cancelCurrentImageLoad;

- (void)sd_cancelCurrentAnimationImagesLoad;//取消动画下载

/**
 *  Show activity UIActivityIndicatorView
 */
- (void)setShowActivityIndicatorView:(BOOL)show;//是否显示加载视图

/**
 *  set desired UIActivityIndicatorViewStyle
 *
 *  @param style The style of the UIActivityIndicatorView
 */
- (void)setIndicatorStyle:(UIActivityIndicatorViewStyle)style;//设置网络提示样式

@end


@interface UIImageView (WebCacheDeprecated)

- (NSURL *)imageURL __deprecated_msg("Use `sd_imageURL`");

- (void)setImageWithURL:(NSURL *)url __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:`");
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:placeholderImage:`");
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:placeholderImage:options`");

- (void)setImageWithURL:(NSURL *)url completed:(SDWebImageCompletedBlock)completedBlock __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:completed:`");
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder completed:(SDWebImageCompletedBlock)completedBlock __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:placeholderImage:completed:`");
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options completed:(SDWebImageCompletedBlock)completedBlock __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:placeholderImage:options:completed:`");
- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options progress:(SDWebImageDownloaderProgressBlock)progressBlock completed:(SDWebImageCompletedBlock)completedBlock __deprecated_msg("Method deprecated. Use `sd_setImageWithURL:placeholderImage:options:progress:completed:`");

- (void)sd_setImageWithPreviousCachedImageWithURL:(NSURL *)url andPlaceholderImage:(UIImage *)placeholder options:(SDWebImageOptions)options progress:(SDWebImageDownloaderProgressBlock)progressBlock completed:(SDWebImageCompletionBlock)completedBlock __deprecated_msg("Method deprecated. Use `sd_setImageWithPreviousCachedImageWithURL:placeholderImage:options:progress:completed:`");

- (void)setAnimationImagesWithURLs:(NSArray *)arrayOfURLs __deprecated_msg("Use `sd_setAnimationImagesWithURLs:`");

- (void)cancelCurrentArrayLoad __deprecated_msg("Use `sd_cancelCurrentAnimationImagesLoad`");

- (void)cancelCurrentImageLoad __deprecated_msg("Use `sd_cancelCurrentImageLoad`");

@end
