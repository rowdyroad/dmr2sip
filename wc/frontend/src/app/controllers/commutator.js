'use strict';

angular
.module('ac.controllers',[])
.controller('CommutatorCtrl', function($scope, $http) {
    function state()
    {
        setTimeout(function() {
            $http.get('/api/commutator/state',{ignoreLoadingBar: true}).success(function(data) {
                $scope.state = data;
                state();
            });
        },1000);
    }
    state();

    $scope.reload = function()
    {
       $http.post('/api/commutator/reload');
    }
});