# JNA 同态加密库集成指南

## 概述

本项目使用 JNA (Java Native Access) 调用 C/C++ 编写的同态加密 .so 库。

## 核心文件

1. **CryptoLibrary.java** - JNA接口定义
2. **CryptoService.java** - 加密服务封装
3. **application.yml** - 配置文件

## 集成步骤

### 步骤1: 准备.so库文件

假设你的同态加密库为 `libcrypto.so`，包含以下函数：

```c
// 初始化
int crypto_init();

// 加密
int crypto_encrypt(const char* plaintext, char* ciphertext, int length);

// 解密
int crypto_decrypt(const char* ciphertext, char* plaintext, int length);

// 批量加密
int crypto_encrypt_batch(char** plaintexts, int count, char** ciphertexts);

// 批量解密
int crypto_decrypt_batch(char** ciphertexts, int count, char** plaintexts);

// 密态查询
int crypto_query(char** encrypted_ids, int count, int* results);

// 清理
void crypto_cleanup();
```

### 步骤2: 放置.so库文件

**Linux:**
```bash
# 复制到系统库目录
sudo cp libcrypto.so /usr/local/lib/

# 或者添加到LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH
```

**Windows:**
```cmd
# 将.dll文件放到以下任意位置：
# 1. 项目的根目录
# 2. C:\Windows\System32
# 3. 添加到PATH环境变量
```

### 步骤3: 修改 CryptoLibrary.java

根据实际.so库的函数签名修改接口定义：

```java
public interface CryptoLibrary extends Library {
    // 根据实际库名加载（不含lib前缀和.so后缀）
    CryptoLibrary INSTANCE = Native.load("crypto", CryptoLibrary.class);
    
    // 根据实际函数签名定义方法
    int crypto_init();
    int crypto_encrypt(String plaintext, byte[] ciphertext, int length);
    // ... 其他方法
}
```

### 步骤4: 配置库路径

修改 `application.yml`：

```yaml
blacklist:
  crypto:
    library-path: /usr/local/lib/libcrypto.so  # Linux
    # library-path: C:/path/to/crypto.dll     # Windows
```

### 步骤5: 实现 CryptoService

根据实际接口调整 `CryptoService.java`：

```java
@Service
public class CryptoService {
    
    @PostConstruct
    public void init() {
        // 设置库路径
        System.setProperty("jna.library.path", libraryPath);
        
        // 初始化
        int result = CryptoLibrary.INSTANCE.crypto_init();
        if (result != 0) {
            throw new RuntimeException("加密库初始化失败");
        }
    }
    
    public String encrypt(String idCard) {
        byte[] ciphertext = new byte[1024];
        int result = CryptoLibrary.INSTANCE.crypto_encrypt(
            idCard, ciphertext, ciphertext.length
        );
        
        if (result == 0) {
            return Base64.getEncoder().encodeToString(ciphertext);
        }
        throw new RuntimeException("加密失败");
    }
}
```

## 常见数据类型映射

### Java → C 类型映射

| Java类型 | C类型 | 说明 |
|---------|------|------|
| String | const char* | 字符串 |
| byte[] | char* | 字节数组 |
| int | int | 整数 |
| long | long | 长整数 |
| Pointer | void* | 指针 |
| String[] | char** | 字符串数组 |

### 指针使用示例

```java
// 输出参数（指针）
public interface CryptoLibrary extends Library {
    // C: int func(char* output)
    int func(byte[] output);
    
    // C: int func(char** outputs, int count)
    int func(Pointer outputs, int count);
}
```

## 完整示例

### C库函数（libcrypto.so）

```c
// crypto.h
int crypto_encrypt(const char* plaintext, char* ciphertext, int length);
```

### JNA接口定义

```java
public interface CryptoLibrary extends Library {
    CryptoLibrary INSTANCE = Native.load("crypto", CryptoLibrary.class);
    
    int crypto_encrypt(String plaintext, byte[] ciphertext, int length);
}
```

### Java调用

```java
String plaintext = "110101199001011234";
byte[] ciphertext = new byte[1024];

int result = CryptoLibrary.INSTANCE.crypto_encrypt(
    plaintext, ciphertext, ciphertext.length
);

if (result == 0) {
    String encrypted = Base64.getEncoder().encodeToString(ciphertext);
    System.out.println("加密成功: " + encrypted);
}
```

## 批量处理示例

### C库函数

```c
int crypto_encrypt_batch(char** plaintexts, int count, char** ciphertexts);
```

### Java调用

```java
// 方式1: 使用StringArray
StringArray plaintexts = new StringArray(new String[]{
    "110101199001011234",
    "110102199002021234"
});

Pointer ciphertexts = new Memory(2 * 1024);
int result = CryptoLibrary.INSTANCE.crypto_encrypt_batch(
    plaintexts, 2, ciphertexts
);

// 方式2: 循环调用单个加密（推荐）
List<String> plaintextList = Arrays.asList("id1", "id2");
List<String> encryptedList = new ArrayList<>();

for (String plaintext : plaintextList) {
    byte[] ciphertext = new byte[1024];
    CryptoLibrary.INSTANCE.crypto_encrypt(
        plaintext, ciphertext, ciphertext.length
    );
    encryptedList.add(Base64.getEncoder().encodeToString(ciphertext));
}
```

## 测试

### 单元测试

```java
@Test
public void testEncrypt() {
    CryptoService cryptoService = new CryptoService();
    cryptoService.init();
    
    String plaintext = "110101199001011234";
    String encrypted = cryptoService.encrypt(plaintext);
    
    assertNotNull(encrypted);
    assertTrue(encrypted.length() > 0);
}
```

### 手动测试

```java
public static void main(String[] args) {
    try {
        // 初始化
        int initResult = CryptoLibrary.INSTANCE.crypto_init();
        System.out.println("初始化结果: " + initResult);
        
        // 加密测试
        String plaintext = "110101199001011234";
        byte[] ciphertext = new byte[1024];
        int encryptResult = CryptoLibrary.INSTANCE.crypto_encrypt(
            plaintext, ciphertext, ciphertext.length
        );
        System.out.println("加密结果: " + encryptResult);
        
        // 清理
        CryptoLibrary.INSTANCE.crypto_cleanup();
    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

## 故障排查

### 1. UnsatisfiedLinkError

**错误信息:**
```
java.lang.UnsatisfiedLinkError: Unable to load library 'crypto'
```

**解决方案:**
```bash
# 检查.so文件是否存在
ls -l /usr/local/lib/libcrypto.so

# 检查库依赖
ldd /usr/local/lib/libcrypto.so

# 设置库路径
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# 或在代码中设置
System.setProperty("jna.library.path", "/usr/local/lib");
```

### 2. 函数签名不匹配

**错误信息:**
```
java.lang.UnsatisfiedLinkError: Error looking up function 'crypto_encrypt'
```

**解决方案:**
- 检查函数名是否正确（区分大小写）
- 检查参数类型是否匹配
- 使用 `nm -D libcrypto.so` 查看导出的符号

### 3. 段错误 (Segmentation Fault)

**原因:**
- 内存越界
- 空指针
- 缓冲区太小

**解决方案:**
```java
// 确保缓冲区足够大
byte[] ciphertext = new byte[2048]; // 增加缓冲区大小

// 检查返回值
int result = CryptoLibrary.INSTANCE.crypto_encrypt(...);
if (result != 0) {
    throw new RuntimeException("加密失败，错误码: " + result);
}
```

## 性能优化

### 1. 批量调用

```java
// 不推荐：逐个调用
for (String id : idCards) {
    encrypt(id);
}

// 推荐：使用批量接口
encryptBatch(idCards);
```

### 2. 缓冲区复用

```java
// 复用缓冲区
byte[] buffer = new byte[1024];
for (String id : idCards) {
    CryptoLibrary.INSTANCE.crypto_encrypt(id, buffer, buffer.length);
    // 处理结果
    Arrays.fill(buffer, (byte) 0); // 清空缓冲区
}
```

### 3. 异步处理

```java
@Async
public CompletableFuture<List<String>> encryptBatchAsync(List<String> idCards) {
    return CompletableFuture.completedFuture(encryptBatch(idCards));
}
```

## 注意事项

1. **线程安全**: 确认.so库是否线程安全
2. **内存管理**: 及时释放申请的内存
3. **错误处理**: 检查所有返回值
4. **资源清理**: 在@PreDestroy中调用cleanup
5. **编码问题**: 注意字符串编码（UTF-8）

## 参考资源

- [JNA官方文档](https://github.com/java-native-access/jna)
- [JNA类型映射](https://github.com/java-native-access/jna/blob/master/www/Mappings.md)
