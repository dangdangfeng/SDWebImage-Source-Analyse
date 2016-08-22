/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"

typedef NS_ENUM(NSInteger, SDImageCacheType) {
    /**
     * The image wasn't available the SDWebImage caches, but was downloaded from the web.
     */
    SDImageCacheTypeNone,//不使用缓存
    /**
     * The image was obtained from the disk cache.
     */
    SDImageCacheTypeDisk,// 内存缓存
    /**
     * The image was obtained from the memory cache.
     */
    SDImageCacheTypeMemory// 磁盘缓存
};
//回调函数
typedef void(^SDWebImageQueryCompletedBlock)(UIImage *image, SDImageCacheType cacheType);

typedef void(^SDWebImageCheckCacheCompletionBlock)(BOOL isInCache);

typedef void(^SDWebImageCalculateSizeBlock)(NSUInteger fileCount, NSUInteger totalSize);

/**
 * SDImageCache maintains a memory cache and an optional disk cache. Disk cache write operations are performed
 * asynchronous so it doesn’t add unnecessary latency to the UI.
 */
@interface SDImageCache : NSObject

/**
 * Decompressing images that are downloaded and cached can improve performance but can consume lot of memory.
 * Defaults to YES. Set this to NO if you are experiencing a crash due to excessive memory consumption.
 */
@property (assign, nonatomic) BOOL shouldDecompressImages;//是否在缓存之前解压图片，此项操作可以提升性能，但是会消耗较多的内存，默认是YES。注意:如果内存不足，可以置为NO

/**
 *  disable iCloud backup [defaults to YES]
 */
@property (assign, nonatomic) BOOL shouldDisableiCloud;//是否禁止iCloud备份，默认是YES

/**
 * use memory cache [defaults to YES]
 */
@property (assign, nonatomic) BOOL shouldCacheImagesInMemory;//是否启用内存缓存 默认是YES

/**
 * The maximum "total cost" of the in-memory image cache. The cost function is the number of pixels held in memory.
 */
@property (assign, nonatomic) NSUInteger maxMemoryCost;//内存最大容量

/**
 * The maximum number of objects the cache should hold.
 */
@property (assign, nonatomic) NSUInteger maxMemoryCountLimit;//内存对象的最大数目

/**
 * The maximum length of time to keep an image in the cache, in seconds
 */
@property (assign, nonatomic) NSInteger maxCacheAge;//磁盘缓存保留的最长时间

/**
 * The maximum size of the cache, in bytes.
 */
@property (assign, nonatomic) NSUInteger maxCacheSize;//磁盘缓存最大容量，以字节为单位

/**
 * Returns global shared cache instance
 *
 * @return SDImageCache global instance
 */
+ (SDImageCache *)sharedImageCache;//返回缓存对象的单例

/**
 * Init a new cache store with a specific namespace
 *
 * @param ns The namespace to use for this cache store
 */
- (id)initWithNamespace:(NSString *)ns;//以ns为缓存空间名字初始化缓存

/**
 * Init a new cache store with a specific namespace and directory
 *
 * @param ns        The namespace to use for this cache store
 * @param directory Directory to cache disk images in
 */
- (id)initWithNamespace:(NSString *)ns diskCacheDirectory:(NSString *)directory;//在directory目录下，以ns为缓存空间名字初始化缓存

-(NSString *)makeDiskCachePath:(NSString*)fullNamespace;//返回磁盘缓存空间的路径

/**
 * Add a read-only cache path to search for images pre-cached by SDImageCache
 * Useful if you want to bundle pre-loaded images with your app
 *
 * @param path The path to use for this read-only cache path
 */
- (void)addReadOnlyCachePath:(NSString *)path;//添加只读内存空间路径，一般用在图片已经下载置相应的缓存目录

/**
 * Store an image into memory and disk cache at the given key.
 *
 * @param image The image to store
 * @param key   The unique image cache key, usually it's image absolute URL
 */
- (void)storeImage:(UIImage *)image forKey:(NSString *)key;//以key为键值将图片image存储置缓存中

/**
 * Store an image into memory and optionally disk cache at the given key.
 *
 * @param image  The image to store
 * @param key    The unique image cache key, usually it's image absolute URL
 * @param toDisk Store the image to disk cache if YES
 */
- (void)storeImage:(UIImage *)image forKey:(NSString *)key toDisk:(BOOL)toDisk;//以key为键值将图片image存储置缓存中,toDisk控制是否写入磁盘缓存

/**
 * Store an image into memory and optionally disk cache at the given key.
 *
 * @param image       The image to store
 * @param recalculate BOOL indicates if imageData can be used or a new data should be constructed from the UIImage
 * @param imageData   The image data as returned by the server, this representation will be used for disk storage
 *                    instead of converting the given image object into a storable/compressed image format in order
 *                    to save quality and CPU
 * @param key         The unique image cache key, usually it's image absolute URL
 * @param toDisk      Store the image to disk cache if YES
 */
- (void)storeImage:(UIImage *)image recalculateFromImage:(BOOL)recalculate imageData:(NSData *)imageData forKey:(NSString *)key toDisk:(BOOL)toDisk;//以key为键值将图片image存储置缓存中,toDisk控制是否写入磁盘缓存,此外如果recalculate为YES或imageData有数据，则将imageData存储置磁盘缓存中

/**
 * Store image NSData into disk cache at the given key.
 *
 * @param imageData The image data to store
 * @param key   The unique image cache key, usually it's image absolute URL
 */
- (void)storeImageDataToDisk:(NSData *)imageData forKey:(NSString *)key;//以key为键值将图片imageData存储置缓存中
/**
 * Query the disk cache asynchronously.
 *
 * @param key The unique key used to store the wanted image
 */
- (NSOperation *)queryDiskCacheForKey:(NSString *)key done:(SDWebImageQueryCompletedBlock)doneBlock;//在内存或磁盘缓存中以key为键值查找图片缓存，如果找到则执行doneBlock回调

/**
 * Query the memory cache synchronously.
 *
 * @param key The unique key used to store the wanted image
 */
- (UIImage *)imageFromMemoryCacheForKey:(NSString *)key;//在内存缓存中查找图片缓存，并返回图片对象

/**
 * Query the disk cache synchronously after checking the memory cache.
 *
 * @param key The unique key used to store the wanted image
 */
- (UIImage *)imageFromDiskCacheForKey:(NSString *)key;//在内存缓存中查找图片缓存，并返回图片对象

/**
 * Remove the image from memory and disk cache asynchronously
 *
 * @param key The unique image cache key
 */
- (void)removeImageForKey:(NSString *)key;//在内存或硬盘缓存中删除指定key缓存


/**
 * Remove the image from memory and disk cache asynchronously
 *
 * @param key             The unique image cache key
 * @param completion      An block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(NSString *)key withCompletion:(SDWebImageNoParamsBlock)completion;//在内存或硬盘缓存中删除指定key缓存,完成后执行响应回调

/**
 * Remove the image from memory and optionally disk cache asynchronously
 *
 * @param key      The unique image cache key
 * @param fromDisk Also remove cache entry from disk if YES
 */
- (void)removeImageForKey:(NSString *)key fromDisk:(BOOL)fromDisk;//在内存或硬盘缓存中删除指定key缓存，fromDisk控制是否删除磁盘缓存对象

/**
 * Remove the image from memory and optionally disk cache asynchronously
 *
 * @param key             The unique image cache key
 * @param fromDisk        Also remove cache entry from disk if YES
 * @param completion      An block that should be executed after the image has been removed (optional)
 */
- (void)removeImageForKey:(NSString *)key fromDisk:(BOOL)fromDisk withCompletion:(SDWebImageNoParamsBlock)completion;//在内存或硬盘缓存中删除指定key缓存,完成后执行响应回调，fromDisk控制是否删除磁盘缓存对象

/**
 * Clear all memory cached images
 */
- (void)clearMemory;//清除内存缓存

/**
 * Clear all disk cached images. Non-blocking method - returns immediately.
 * @param completion    An block that should be executed after cache expiration completes (optional)
 */
- (void)clearDiskOnCompletion:(SDWebImageNoParamsBlock)completion;//清除磁盘缓存，完成后执行回调

/**
 * Clear all disk cached images
 * @see clearDiskOnCompletion:
 */
- (void)clearDisk;//清除磁盘缓存

/**
 * Remove all expired cached image from disk. Non-blocking method - returns immediately.
 * @param completionBlock An block that should be executed after cache expiration completes (optional)
 */
- (void)cleanDiskWithCompletionBlock:(SDWebImageNoParamsBlock)completionBlock;//清除过期缓存，如果缓存容量超过限制，则清除部分缓存直至达到预期目标为止

/**
 * Remove all expired cached image from disk
 * @see cleanDiskWithCompletionBlock:
 */
- (void)cleanDisk;

/**
 * Get the size used by the disk cache
 */
- (NSUInteger)getSize;//返回磁盘缓存的大小

/**
 * Get the number of images in the disk cache
 */
- (NSUInteger)getDiskCount;//返回磁盘缓存对象的数目

/**
 * Asynchronously calculate the disk cache's size.
 */
- (void)calculateSizeWithCompletionBlock:(SDWebImageCalculateSizeBlock)completionBlock;//异步计算磁盘缓存所需大小

/**
 *  Async check if image exists in disk cache already (does not load the image)
 *
 *  @param key             the key describing the url
 *  @param completionBlock the block to be executed when the check is done.
 *  @note the completion block will be always executed on the main queue
 */
- (void)diskImageExistsWithKey:(NSString *)key completion:(SDWebImageCheckCacheCompletionBlock)completionBlock;//异步查看磁盘缓存中是否存在指定key的图片，完成后执行回调

/**
 *  Check if image exists in disk cache already (does not load the image)
 *
 *  @param key the key describing the url
 *
 *  @return YES if an image exists for the given key
 */
- (BOOL)diskImageExistsWithKey:(NSString *)key;

/**
 *  Get the cache path for a certain key (needs the cache path root folder)
 *
 *  @param key  the key (can be obtained from url using cacheKeyForURL)
 *  @param path the cache path root folder
 *
 *  @return the cache path
 */
- (NSString *)cachePathForKey:(NSString *)key inPath:(NSString *)path;//返货指定路径path下的key对象的缓存路径

/**
 *  Get the default cache path for a certain key
 *
 *  @param key the key (can be obtained from url using cacheKeyForURL)
 *
 *  @return the default cache path
 */
- (NSString *)defaultCachePathForKey:(NSString *)key;//返回默认路径下key对象的缓存路径

@end
