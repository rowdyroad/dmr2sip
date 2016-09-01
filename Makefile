PROJECT=dmr2sip
DOCKER=docker
EXEC=$(DOCKER) exec
BUILD=$(DOCKER) build
RUN=$(DOCKER) run
STOP=$(DOCKER) stop
REMOVE=$(DOCKER) rm
BACKEND=$(PROJECT)-backend
FRONTEND=$(PROJECT)-frontend
DATABASE=$(PROJECT)-database
SERVICE=$(PROJECT)-service
SANDBOX=$(PROJECT)-sandbox


docker-purge:
	-docker rm $$(docker ps -q -f status=exited -f status=created -f status=restarting -f status=paused)

wc: wc-backend-image wc-frontend-image run-db run-wc-backend composer migrate run-wc-frontend prepare-front front

wc-backend-image: 
	$(BUILD) -t $(BACKEND)-image deploy/backend

wc-frontend-image:
	$(BUILD) -t $(FRONTEND)-image deploy/frontend

run-wc: run-db run-wc-backend run-wc-frontend

run-db: docker-purge
	docker ps -f status=running | grep $(DATABASE) || \
	$(RUN) -d --name $(DATABASE) \
		-v $(PWD)/db:/var/lib/mysql \
		-p 3306:3306 \
		-e MYSQL_ROOT_PASSWORD=root \
		-e MYSQL_DATABASE=db \
		-t mysql \
		--character-set-server=utf8mb4 \
		--collation-server=utf8mb4_unicode_ci
stop-db:
	-$(STOP) $(DATABASE)
	-$(REMOVE) $(DATABASE)

run-wc-backend: docker-purge
	docker ps -f status=running | grep $(BACKEND) || \
	$(RUN) -d --name $(BACKEND) \
		-v $(PWD)/wc/backend:/opt/backend \
		-v $(PWD)/wc/upload:/opt/upload \
		-v /var/run:/var/run/docker \
		-e "COMMUTATOR_SERVICE=$(SERVICE)" \
		-p 8022:80 \
		--link $(DATABASE):mysql \
		-t $(BACKEND)-image

stop-wc-backend:
	-$(STOP) $(BACKEND)
	-$(REMOVE) $(BACKEND)

run-wc-frontend: docker-purge
	docker ps -f status=running | grep $(FRONTEND) || \
	$(RUN) -d --name $(FRONTEND) \
		-v $(PWD)/wc/frontend:/opt/frontend \
		-p 8023:80 \
		--link $(BACKEND):backend \
		-t $(FRONTEND)-image

stop-wc-frontend:
	-$(STOP) $(FRONTEND)
	-$(REMOVE) $(FRONTEND)

migrate:
	$(EXEC) -it $(BACKEND) bash -c "cd /opt/backend && ./yii migrate --interactive=0"

migrate-create:
	$(EXEC) -it $(BACKEND) bash -c "cd /opt/backend && ./yii migrate/create --interactive=0 $(name)"

prepare-front:
	$(EXEC) -it $(FRONTEND) bash -c "cd /opt/frontend && npm update && bower update --allow-root"

front:
	$(EXEC) -it $(FRONTEND) bash -c "cd /opt/frontend && gulp"

composer:
	$(EXEC) -it $(BACKEND) bash -c "cd /opt/backend && composer update"

sandbox: sandbox-image run-sandbox

sandbox-image:
	$(BUILD) -t $(SANDBOX)-image -f deploy/commutator/Dockerfile.sandbox deploy/commutator

run-sandbox: docker-purge run-db
	docker ps -f status=running | grep $(SANDBOX) || \
	$(RUN) -d --name $(SANDBOX) \
		-v $(PWD)/commutator:/opt \
		--link $(DATABASE):mysql \
		-t $(SANDBOX)-image

stop-sandbox:
	-$(STOP) $(SANDBOX)
	-$(REMOVE) $(SANDBOX)

commutator: docker-purge
	docker ps -f status=running | grep $(SANDBOX) || make sandbox
	$(EXEC) -it $(SANDBOX) bash -c "cd /opt && make"

service: service-image commutator run-service

service-image:
	$(BUILD) -t $(SERVICE)-image -f deploy/commutator/Dockerfile.service deploy/commutator

stop-service:
	-$(STOP) $(SERVICE)
	-$(REMOVE) $(SERVICE)

run-service: docker-purge run-db
	docker ps -f status=running | grep $(SERVICE) || \
	$(RUN) -d --name $(SERVICE) \
		-v $(PWD)/commutator/build:/opt/commutator/bin \
		-v $(PWD)/commutator/config:/opt/commutator/etc \
		--device /d4ev/snd \
		--link $(DATABASE):mysql \
		-t $(SERVICE)-image
