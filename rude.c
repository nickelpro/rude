#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const char msg[] =
    "GET /v1/forecast?latitude=40.88&longitude=-73.25&current_weather=true "
    "HTTP/1.0\r\n"
    "Host: api.open-meteo.com\r\n\r\n";
const size_t msg_len = sizeof msg - 1;

int main() {

  // Lookup host information and fill in sockaddr
  struct hostent* host_info = gethostbyname("api.open-meteo.com");
  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(80),
  };
  memcpy(&server_addr.sin_addr.s_addr, host_info->h_addr_list[0],
      host_info->h_length);

  // Open a connection
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sock_fd, (struct sockaddr*) &server_addr, sizeof server_addr);

  // Send request
  for(size_t sent = 0; sent < msg_len;) {
    int result = write(sock_fd, msg + sent, msg_len - sent);
    if(result < 0)
      exit(EXIT_FAILURE);
    sent += result;
  }

  // Read response
  char response[4096] = {0};
  const size_t buf_size = sizeof response;
  size_t used = 0;
  while(used < buf_size) {
    int result = read(sock_fd, response + used, buf_size - used);
    if(result < 0)
      exit(EXIT_FAILURE);
    used += result;
    if(!result)
      break;
  }

  // Scan for second opening "{"
  char* cursor = response;
  for(unsigned count = 0; count < 2; ++cursor) {
    if(*cursor == '{')
      ++count;
  }

  // Skip to the temperature float
  cursor += sizeof "\"temperature\":" - 1;

  // Read and convert to fahrenheit
  double temp = atof(cursor) * 1.8 + 32;

  // Done
  printf("It is currently %.1f° Fahrenheit\n", temp);
}
