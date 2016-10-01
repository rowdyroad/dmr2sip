'use strict';

angular
.module('ac.controllers',['ngRoute'])
.config(function($routeProvider) {
  $routeProvider
  .when('/configuration/service', {
    templateUrl: '/views/configuration/service.html',
    controller: 'StateConfigurationCtrl'
  })
})
.controller('StateCtrl', function($scope, $http,$routeParams, $rootScope) {
	function states()
    {
        setTimeout(function() {
            $http.get('/api/state/states',{ignoreLoadingBar: true}).success(function(data) {
                $scope.state = data;
                $rootScope.$broadcast('points:states', $scope.state.points);
                states();
            });
        },1000);
    }
    states();

})
.controller('StateConfigurationCtrl', function($scope, $http,$routeParams) {
	$scope.reload = function()
	{
		$http.post('/api/state/reload');
	}

	$scope.restart = function()
	{
		$http.post('/api/state/restart');
	}

	$http.get('/api/state/log').success(function(data) {
        $scope.log = data.data;
    });
});