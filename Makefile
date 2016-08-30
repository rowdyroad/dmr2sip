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
COMMUTATOR=$(PROJECT)-commutator
COMMUTATOR_SANDBOX=$(PROJECT)-commutator-sandbox

build: build-backend build-frontend

build-backend: 
	$(BUILD) -t $(BACKEND)-image deploy/backend

build-frontend:
	$(BUILD) -t $(FRONTEND)-image deploy/frontend

run: run-database run-backend composer migrate run-frontend front

rerun: stop-frontend stop-backend stop-database run

run-database:
	$(RUN) -d --name $(DATABASE) \
		-v $(PWD)/db:/var/lib/mysql \
		-p 3306:3306 \
		-e MYSQL_ROOT_PASSWORD=root \
		-e MYSQL_DATABASE=db \
		-t mysql \
		--character-set-server=utf8mb4 \
		--collation-server=utf8mb4_unicode_ci
stop-database:
	-$(STOP) $(DATABASE)
	-$(REMOVE) $(DATABASE)

run-backend:
	$(RUN) -d --name $(BACKEND) \
		-v $(PWD)/wc/backend:/opt/backend \
		-v $(PWD)/wc/upload:/opt/upload \
		-p 8022:80 \
		--link $(DATABASE):mysql \
		-t $(BACKEND)-image
stop-backend:
	-$(STOP) $(BACKEND)
	-$(REMOVE) $(BACKEND)

run-frontend:
	$(RUN) -d --name $(FRONTEND) \
		-v $(PWD)/wc/frontend:/opt/frontend \
		-p 8023:80 \
		--link $(BACKEND):backend \
		-t $(FRONTEND)-image
stop-frontend:
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

build-commutator-sandbox:
	$(BUILD) -t $(COMMUTATOR_SANDBOX)-image -f deploy/commutator/Dockerfile.sandbox deploy/commutator

run-commutator-sandbox:
	$(RUN) -d --name $(COMMUTATOR_SANDBOX) -v $(PWD)/commutator:/opt --link $(DATABASE):mysql -t $(COMMUTATOR_SANDBOX)-image

stop-commutator-sandbox:
	-$(STOP) $(COMMUTATOR_SANDBOX)
	-$(REMOVE) $(COMMUTATOR_SANDBOX)

commutator:
	$(EXEC) -it $(COMMUTATOR_SANDBOX) bash -c "cd /opt && make"

build-commutator:
	$(BUILD) -t $(COMMUTATOR)-image -f deploy/commutator/Dockerfile.service deploy/commutator

stop-commutator:
	-$(STOP) $(COMMUTATOR)
	-$(REMOVE) $(COMMUTATOR)

run-commutator:
	$(RUN) -d --name $(COMMUTATOR) \
	    -v $(PWD)/commutator/build:/opt/commutator/bin \
	    -v $(PWD)/commutator/config:/opt/commutator/etc \
	    --device /dev/snd \
	    --link $(DATABASE):mysql \
	    -t $(COMMUTATOR)-image
