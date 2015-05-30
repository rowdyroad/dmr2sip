function AuthCtrl($scope, $location, $route, $http, auth, $rootScope)
{
    $scope.$on('auth:user', function(event, user) {
        $scope.user = user;
    });

    $scope.credentials = {};
    $scope.login = function()
    {
        auth.Login($scope.credentials.username, $scope.credentials.password);
    }

    $scope.logout = function()
    {
        auth.Logout();
    }
}