FROM ubuntu:16.04

RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y git nginx curl && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*


RUN ln -sf /dev/stdout /var/log/nginx/access.log && \
    ln -sf /dev/stderr /var/log/nginx/error.log

RUN curl -sL https://deb.nodesource.com/setup_4.x | bash - && \
	apt-get install -y nodejs

RUN npm install -g gulp-cli bower

COPY etc/nginx.conf /etc/nginx/sites-available/default

CMD ["/usr/sbin/nginx", "-g", "daemon off;"]
EXPOSE 80