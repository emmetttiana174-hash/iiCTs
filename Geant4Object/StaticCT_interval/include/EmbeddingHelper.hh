#ifndef EMBEDDINGHELPER_HH
#define EMBEDDINGHELPER_HH

/**
 * @brief 检查命令行参数并以嵌入模式运行Geant4会话。
 *
 * 该函数会解析传入的命令行参数。如果找到了 "--embed-mode" 标志，
 * 它将接管程序的执行，启动一个用于嵌入的特殊Qt会话，并最终返回退出码。
 * 如果没有找到该标志，函数会立即返回 -1，表示主程序应该继续执行其
 * 常规的独立运行逻辑。
 *
 * @param argc 命令行参数计数 (来自main函数)
 * @param argv 命令行参数数组 (来自main函数)
 * @return int 如果进入嵌入模式，则返回Qt应用的退出码；否则返回 -1。
 */
int runInEmbeddedMode(int argc, char* argv[]);

#endif // EMBEDDINGHELPER_HH