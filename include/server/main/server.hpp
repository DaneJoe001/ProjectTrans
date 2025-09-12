#pragma once

class ServerMainWindow;

void clear_log();

void init_database();

void clear_database();

void run_server();

void stop_server_handler(int signal);