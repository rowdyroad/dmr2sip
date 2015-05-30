'use strict';

Number.prototype.padLeft = function (n,str){
    return Array(n-String(this).length+1).join(str||'0')+this;
}

Date.prototype.toString = function()
{
  return this.getUTCFullYear() + "-"
        + (this.getUTCMonth() + 1).padLeft(2) + "-"
        + this.getUTCDate().padLeft(2) + " "
        + this.getUTCHours().padLeft(2) + ":"
        + this.getUTCMinutes().padLeft(2) + ":"
        + this.getUTCSeconds().padLeft(2);
}


function BaseController(prefix, pkAttribute, $scope, $http, init)
{
    $scope.page = 1;
    $scope.pkAttribute = pkAttribute;
    $scope.items = [];

    $scope.getParams = function() { return  {}; }

    $scope.get = function(clear)
    {
        if (clear) {
            $scope.page = 1;
            $scope.items = [];
        }
        var options = { params: angular.extend({}, $scope.getParams(), {page: $scope.page}) };

        $http.get(prefix, options).success(function(data) {
            if (data.data && data._meta) {
                if ($scope.page <= data._meta.pageCount) {
                    angular.forEach(data.data, function(v) {
                        $scope.items.push(v);
                    });
                    ++$scope.page;
                }
            } else {
                $scope.items = data;
            }
        });
    }

    if (angular.isFunction(init)) {
        init();
    }
    $scope.get();
}

function BaseDeleteFormController(prefix, pkAttribute, $scope, $http, $modal)
{
        $scope.delete = function(item)
        {
            var model = item || $scope.current;
            var modalInstance = $modal.open({
                templateUrl: 'CommonConfirmDialogView',
                controller: function ($scope,$http, $modalInstance, config)
                {
                    $scope.config = config;
                    $scope.confirm = function()
                    {
                        $modalInstance.close();
                    }

                    $scope.cancel = function()
                    {
                        $modalInstance.dismiss('cancel');
                    }
                },
                resolve: {
                    config: function() {  return {
                        title: 'Удаление элемента',
                        text: 'Удалить выбранный элемент?',
                        confirm_btn_class:'btn-danger',
                        confirm_btn_text: 'Удалить'
                    } }
                }
            });

            modalInstance.result.then(function () {
                $http.delete(prefix + '/' + model[$scope.pkAttribute]).success(function() {
                    $scope.$emit('after_delete', model);
                });
            });
        }
}



 function FormController(prefix, pkAttribute, $scope, $http, $routeParams, $modal, item)
 {

    $scope.isNew = item || $routeParams[pkAttribute] ? false : true;
    $scope.current = item || {};
    $scope.pkAttribute = pkAttribute;

    BaseDeleteFormController(prefix, pkAttribute, $scope, $http, $modal);

    angular.forEach($routeParams, function(v, k) {
        if (k != pkAttribute) {
            $scope.current[k] = v;
        }
    });

    if ($routeParams[pkAttribute]) {
        $http.get(prefix +'/' + $routeParams[pkAttribute]).success(function(data) {
            $scope.current = data;
            $scope.$broadcast('after_find', data);
        });
    }

    $scope.$on('after_delete', function() {
       window.history.back();
    });

    $scope.cancel = function()
    {
        window.history.back();
    }

    $scope.beforeSave = function()
    {
      return true;
    }

    $scope.save = function()
    {
        if ($scope.current && $scope.current[$scope.pkAttribute]) {
            if ($scope.beforeSave()) {
              $http.put(prefix + '/' + $scope.current[$scope.pkAttribute], $scope.current).success(function(model) {
                  $scope.current = angular.copy(model);
                  $scope.$emit('after_save', model);
                  window.history.back();
              });
            }
        } else {
            if ($scope.beforeSave()) {
              $http.post(prefix, $scope.current).success(function(model) {
                  $scope.current = angular.copy(model);
                  $scope.$emit('after_insert', model);
                  $scope.$emit('after_save', model);
                  window.history.back();
              });
            }
        }
    }
}

angular.config = {};

angular.module('myApp.Auth', []).controller('AuthCtrl', AuthCtrl);

angular.module('myApp', [
  'ui.bootstrap',
  'angular-growl',
  'ngRoute',
  'myApp.users',
  'myApp.events',
  'myApp.points',
  'myApp.routes',

  'myApp.Auth'
])
.controller('MainFormAttachmentCtrl', function($scope, $http, $routeParams, $rootScope, FileUploader) {
     $scope.uploader = new FileUploader({url: '/api/gallery',autoUpload:true});

     $scope.$on('after_find', function(event, data) {
          var attachments = [];
          angular.forEach(data.attachments, function(a) {
            attachments.push(a.filename);
          });
          $scope.uploader.addToQueue(attachments);
     });

    $scope.uploader.onCompleteItem = function(fileItem, response, status, headers) {

        if (!fileItem.stored_files) {
          fileItem.stored_files = [];
        }
        if (response.length > 0) {
          angular.forEach(response, function(file) {
            fileItem.stored_files.push(file);
          });
        } else {
            fileItem.stored_files.push(fileItem._file);
        }

        $scope.raise();
    };

    $scope.removeAttachment = function(item)
    {
      item.remove();
      $scope.raise();
    }

    $scope.clearAttachments = function()
    {
        uploader.clearQueue();
        $scope.raise();
    }

    $scope.raise = function()
    {
        var model = { attachments: [] };
        angular.forEach($scope.uploader.queue, function(item) {
          if (item.isSuccess) {
            angular.forEach(item.stored_files, function(file) {
              model.attachments.push({filename: file});
            });
          }
        });
        $scope.$emit('update_model', model);
    }
})
.run(['$route', '$rootScope', '$location', 'auth', function ($route, $rootScope, $location, auth) {

    $rootScope.checkAccess = function(permission) {
      if (angular.isArray(permission)) {
        for (var i in permission) {
            if ($rootScope.checkAccess(permission[i])) {
              return true;
            }
        }
        return false;
      }
      return auth.CheckAccess(permission);
    }

    $rootScope.auth = auth;

    var original = $location.path;
    $location.path = function (path, reload) {
        if (reload === false) {
            var lastRoute = $route.current;
            var un = $rootScope.$on('$locationChangeSuccess', function () {
                $route.current = lastRoute;
                un();
            });
        }
        return original.apply($location, [path]);
    };
}])
.service('auth', AuthService)
.directive('onlyFloat', function() {
      return function(scope, element, attrs) {
        var dotKeyCode = 190;
        var minusKeyCode = 189;
         var keyCode = [minusKeyCode, dotKeyCode,8,9,37,39,48,49,50,51,52,53,54,55,56,57,96,97,98,99,100,101,102,103,104,105,110];
          element.bind("keydown", function(event) {
            if (($.inArray(event.which,keyCode) == -1) ||
                (event.which == dotKeyCode && element.val().indexOf('.')!= -1) ||
                (event.which == minusKeyCode && element.val().length )) {

                if (event.which == minusKeyCode && element.val().indexOf('-') == -1) {
                  element.val('-' + element.val());
                }
                scope.$apply(function(){
                    scope.$eval(attrs.onlyNum);
                    event.preventDefault();
                });
                event.preventDefault();
            } else {
              if (event.which == minusKeyCode) {
                  element.val('-' + element.val());
                  event.preventDefault();
              }
            }
        });
     };
})
.directive('onlyInteger', function() {
      return function(scope, element, attrs) {
          var minusKeyCode = 189;
          var keyCode = [minusKeyCode, 8,9,37,39,48,49,50,51,52,53,54,55,56,57,96,97,98,99,100,101,102,103,104,105,110];
          element.bind("keydown", function(event) {
            if (($.inArray(event.which,keyCode) == -1) ||
                (event.which == minusKeyCode && element.val().length )) {

                if (event.which == minusKeyCode && element.val().indexOf('-') == -1) {
                  element.val('-' + element.val());
                }

                scope.$apply(function(){
                    scope.$eval(attrs.onlyNum);
                    event.preventDefault();
                });
                event.preventDefault();
            }
        });
     };
})
.directive('onlyUnsigned', function() {
    var dotKeyCode = 190;

      return function(scope, element, attrs) {
          var keyCode = [dotKeyCode, 8,9,37,39,48,49,50,51,52,53,54,55,56,57,96,97,98,99,100,101,102,103,104,105,110];
          element.bind("keydown", function(event) {
            if (($.inArray(event.which,keyCode) == -1) ||
                 (event.which == dotKeyCode && element.val().indexOf('.')!= -1)) {
                scope.$apply(function(){
                    scope.$eval(attrs.onlyNum);
                    event.preventDefault();
                });
                event.preventDefault();
            }
        });
     };
})
.config(function($httpProvider) {
  $httpProvider.defaults.headers.common["X-Requested-With"] = 'XMLHttpRequest';
     $httpProvider.interceptors.push(function($q, $location, $rootScope,growl) {
      return {
        responseError: function(response) {
          if (response.status == 401 || response.status == 403) {
            if ($location.path() != '/login') {
              $location.path('/login');
            }
            return $q.reject(response);
          }

          var k = "<b>" + response.statusText+'</b>';
          if (response.status == 422) {
              k +='<ul>';
              for (var i in response.data) {
                  k+='<li>' + response.data[i].message + '</li>';
              }
              k+='</ul>';
          }
          growl.error(k, {ttl: 3000});
          return $q.reject(response);
        }
    }});

}).config(['$locationProvider',function($locationProvider){
    $locationProvider.html5Mode(true);
}]).config(['$routeProvider', function($routeProvider) {
  $routeProvider
  .when('/login', {
    templateUrl: '/views/auth.html',
    controller: 'AuthCtrl'
  })
  .otherwise({redirectTo: '/'});
}]);

