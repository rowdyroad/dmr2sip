'use strict';

angular.module('myApp.events', ['ngRoute'])

.config(function($routeProvider) {
  $routeProvider.when('/events', {
    templateUrl: '/views/events.html',
    controller: 'EventCtrl'
  });
})
.controller('EventCtrl', function($scope, $http) {
  BaseController('/api/events', 'event_id', $scope, $http);
});