/*
 * cmd_parser.h
 */
#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// 最大命令长度
#define MAX_CMD_LENGTH 128
#define MAX_ARG_LENGTH 32
#define MAX_ARGS       8 // 最大参数数量

// 命令返回状态
typedef enum {
        CMD_SUCCESS = 0,
        CMD_ERROR_INVALID,
        CMD_ERROR_TOO_LONG,
        CMD_ERROR_MISSING_ARG,
        CMD_ERROR_INVALID_ARG,
        CMD_ERROR_TOO_MANY_ARGS
} cmd_status_t;

// 参数结构
typedef struct {
        uint8_t count;                         // 参数数量
        char values[MAX_ARGS][MAX_ARG_LENGTH]; // 参数值数组
} cmd_args_t;

// 命令处理器函数指针（支持多参数）
typedef cmd_status_t (*cmd_handler_func_t)(cmd_args_t *args, char *response);

// 命令定义结构
typedef struct {
        const char *name;           // 命令名称
        const char *description;    // 命令描述
        cmd_handler_func_t handler; // 命令处理器
} command_def_t;

// 函数声明
void CmdParser_Init(void);
void CmdParser_ProcessChar(uint8_t ch);
void CmdParser_RegisterCommand(const char *name, const char *desc, cmd_handler_func_t handler);
void CmdParser_SendResponse(const char *response);
uint8_t CmdParse_Arguments(const char *input, cmd_args_t *args);

#endif /* __CMD_PARSER_H__ */
