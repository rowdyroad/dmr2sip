FROM ubuntu:16.04

RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y \
	git \
	nginx \
	supervisor \
	sudo \
	net-tools \
	unzip \
	curl \
	cron \
	wget \
	mysql-client \
	php \
	php-cli \
	php-fpm \
	php-curl \
	php-json \
	php-dom \
	php-gd \
	php-mysql \
	php-mcrypt \
	php-mbstring \
	php-intl && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN ln -sf /dev/stdout /var/log/nginx/access.log && \
   ln -sf /dev/stderr /var/log/nginx/error.log


RUN LAST_PHP_VERSION=`ls -1 /etc/php | tail -1` && \
    sed -i -E 's/^listen \= .*$/listen \= \/var\/run\/php-fpm.sock/' /etc/php/$LAST_PHP_VERSION/fpm/pool.d/www.conf && \
    sed -i -E 's/^pid \= .*$/pid \= \/var\/run\/php-fpm.pid/' /etc/php/$LAST_PHP_VERSION/fpm/php-fpm.conf && \
    sed -i -E 's/^error_log \= .*$/error_log \= \/dev\/stderr/' /etc/php/$LAST_PHP_VERSION/fpm/php-fpm.conf && \
    PHP_FPM=`whereis php-fpm | sed "s/^php-fpm: \(.*\)$/\1/"` && \
    ln -fs $PHP_FPM /usr/bin/php-fpm

ENV COMPOSER_HOME /root/.composer
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer && \
    /usr/local/bin/composer global require "fxp/composer-asset-plugin" && \
    /usr/local/bin/composer config -g github-oauth.github.com 20abbd04496a38458a9b52843c07a235a6db60c9

COPY etc/nginx.conf /etc/nginx/sites-available/default
COPY etc/supervisord.conf /etc/supervisord.conf
COPY etc/sudoers.d /etc/sudoers.d/

CMD ["/usr/bin/supervisord", "-n" ,"-c","/etc/supervisord.conf"]

EXPOSE 80
EXPOSE 443

