/*
 * @Description: 
 * @Author: melt
 * @Date: 2020-05-30 21:22:10
 * @LastEditTime: 2020-05-30 22:10:43
 */ 
/*************************************************************************
	> File Name: signal_set.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2020年05月30日 星期六 21时21分20秒
 ************************************************************************/

#include<iostream>

#include<signal.h>
#include<unistd.h>

void sighandle( int sig ) {
	std::cout << "capture signal: " << sig << std::endl;	
}
void set_signal() {
	
	signal( SIGINT, sighandle);
	signal( SIGQUIT, sighandle); 
	signal( SIGHUP, sighandle );
}
void set_sigaction() {
	struct sigaction act;
	struct sigaction oact;

	act.sa_flags = 0;
	act.sa_handler = sighandle;
	sigfillset( &act.sa_mask );

	sigaction( SIGINT, &act, &oact );
	
}
int main( int argc, char* argv[]) {
	
	set_sigaction();

	pause();

	return 0;

}