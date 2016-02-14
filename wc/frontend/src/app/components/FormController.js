'use strict'
function FormController(prefix, pkAttribute, $scope, $http, $routeParams, $modal, item)
 {
    var query_params = {};
    if (typeof pkAttribute === 'object') {
      query_params = pkAttribute.params;
      pkAttribute = pkAttribute.pkAttribute;
    }

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
        $http.get(prefix +'/' + $routeParams[pkAttribute], {params:query_params}).success(function(data) {
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


    $scope.save = function()
    {
        var beforeSaveFlag = !$scope.beforeSave || ($scope.beforeSave && $scope.beforeSave());

        if ($scope.current && $scope.current[$scope.pkAttribute]) {
            if (beforeSaveFlag) {
              $http({
                      method: 'PUT',
                      url: prefix + '/' + $scope.current[$scope.pkAttribute],
                      data: $scope.current
              })
              .success(function(model) {
                  $scope.current = angular.copy(model);
                  $scope.$emit('after_save', model);
                  window.history.back();
              });
            }
        } else {
            if (beforeSaveFlag) {
              $http({
                    method: 'POST',
                    url: prefix,
                    data: $scope.current
                  }).success(function(model) {
                    $scope.current = angular.copy(model);
                    $scope.$emit('after_insert', model);
                    $scope.$emit('after_save', model);
                    window.history.back();
              });
            }
        }
    }
}