#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 1000 // 假设每行最大长度为1000个字符
#define NUM_THREADS 4        // 假设使用4个线程进行并行处理
#define MAX_CHUNK_SIZE 10000000 // 假设每个块最多包含1000万行数据

typedef struct {
	char **lines;
	int lineCount;
} ChunkData;

// 比较函数，用于qsort排序
int compare(const void *a, const void *b) {
	return strcmp(*(const char **)a, *(const char **)b);
}

// 将排序好的数据写入文件
void writeToFile(const char *filename, char **lines, int count) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < count; ++i) {
		fputs(lines[i], file);
	}
	fclose(file);
}

// 外部排序处理函数
void *externalSortThread(void *chunkData) {
	ChunkData *data = (ChunkData *)chunkData;
	
	// 使用快速排序算法对数据进行排序
	qsort(data->lines, data->lineCount, sizeof(char *), compare);
	
	pthread_exit(NULL);
}

// 将文件分块并排序
void sortChunks(const char *inputFilename, int numThreads) {
	FILE *inputFile = fopen(inputFilename, "r");
	if (inputFile == NULL) {
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}
	
	char buffer[MAX_LINE_LENGTH];
	int lineCount = 0;
	int chunkNum = 0;
	char **lines = (char **)malloc(MAX_CHUNK_SIZE * sizeof(char *)); // 分配内存来存储当前块的数据
	
	// 创建线程数组
	pthread_t threads[numThreads];
	int threadArgs[numThreads];
	
	// 逐行读取文件内容
	while (fgets(buffer, MAX_LINE_LENGTH, inputFile) != NULL) {
		// 假设文件中没有空行
		lines[lineCount] = strdup(buffer);
		if (lines[lineCount] == NULL) {
			perror("Memory allocation error");
			exit(EXIT_FAILURE);
		}
		++lineCount;
		if (lineCount == MAX_CHUNK_SIZE) {
			// 每读取一块数据就启动一个线程进行排序
			ChunkData *data = (ChunkData *)malloc(sizeof(ChunkData));
			data->lines = lines;
			data->lineCount = lineCount;
			
			threadArgs[chunkNum] = pthread_create(&threads[chunkNum], NULL, externalSortThread, (void *)data);
			if (threadArgs[chunkNum]) {
				printf("Error: Unable to create thread %d\n", threadArgs[chunkNum]);
				exit(EXIT_FAILURE);
			}
			
			// 清空缓冲区，准备读取下一块数据
			lineCount = 0;
			lines = (char **)malloc(MAX_CHUNK_SIZE * sizeof(char *));
			if (lines == NULL) {
				perror("Memory allocation error");
				exit(EXIT_FAILURE);
			}
			chunkNum++;
		}
	}
	fclose(inputFile);
	
	// 等待所有线程结束
	for (int i = 0; i < chunkNum; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// 对剩余的数据进行排序
	if (lineCount > 0) {
		ChunkData *data = (ChunkData *)malloc(sizeof(ChunkData));
		data->lines = lines;
		data->lineCount = lineCount;
		externalSortThread((void *)data);
		free(data);
	}
}
// 合并排序后的块
void mergeChunks(const char *outputFilename, int numChunks) {
	// 创建一个文件指针数组，用于存储每个块的文件指针
	FILE *chunkFiles[numChunks];
	
	// 打开每个块文件
	for (int i = 0; i < numChunks; i++) {
		char chunkFilename[20];
		sprintf(chunkFilename, "chunk_%d.txt", i);
		chunkFiles[i] = fopen(chunkFilename, "r");
		if (chunkFiles[i] == NULL) {
			perror("Error opening chunk file");
			exit(EXIT_FAILURE);
		}
	}
	
	// 创建一个输出文件指针
	FILE *outputFile = fopen(outputFilename, "w");
	if (outputFile == NULL) {
		perror("Error opening output file");
		exit(EXIT_FAILURE);
	}
	
	// 创建一个数组来存储每个块当前的行
	char currentLines[numChunks][MAX_LINE_LENGTH];
	for (int i = 0; i < numChunks; i++) {
		if (fgets(currentLines[i], MAX_LINE_LENGTH, chunkFiles[i]) == NULL) {
			// 如果块文件为空，则将对应的currentLines设置为NULL
			currentLines[i][0] = '\0';
		}
	}
	
	// 合并排序后的块
	while (1) {
		int minIndex = -1;
		for (int i = 0; i < numChunks; i++) {
			if (currentLines[i][0] != '\0') {
				if (minIndex == -1 || strcmp(currentLines[i], currentLines[minIndex]) < 0) {
					minIndex = i;
				}
			}
		}
		if (minIndex == -1) {
			// 所有块都处理完毕
			break;
		}
		// 将当前最小的行写入输出文件
		fputs(currentLines[minIndex], outputFile);
		// 读取下一行数据
		if (fgets(currentLines[minIndex], MAX_LINE_LENGTH, chunkFiles[minIndex]) == NULL) {
			// 如果已经到达文件末尾，则将currentLines[minIndex]设置为空字符串
			currentLines[minIndex][0] = '\0';
		}
	}
	
	// 关闭所有块文件和输出文件
	for (int i = 0; i < numChunks; i++) {
		fclose(chunkFiles[i]);
	}
	fclose(outputFile);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	const char *inputFilename = argv[1];
	const char *outputFilename = argv[2];
	
	// 使用多线程对文件分块并进行排序
	sortChunks(inputFilename, NUM_THREADS);
	
	// 多线程合并排序后的块
	mergeChunks(outputFilename, NUM_THREADS);
	
	printf("External sorting completed successfully!\n");
	
}
