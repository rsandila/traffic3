angular.module("traffic3App")
  .controller("clientCtrl", function ($scope, $http, $location, $route, $interval) {
    $scope.data = {};
    $scope.addActive = 0;
    $scope.protocols = {};
    $scope.contentmanagers = {};

    $scope.add = {
      id: 0,
      port: 10000,
      min: 100,
      max: 1000,
      count: 1
    };
    $http.get("/status/supports/protocol").success(function (data) {
      $scope.protocols = data;
    }).error(function (error) {
      $scope.error = error;
    });


    $http.get("/status/supports/contentmanager").success(function (data) {
      $scope.contentmanagers = data;
    }).error(function (error) {
      $scope.error = error;
    });


    $scope.updateStatus = function() {
      $http.get("/client").success(function (data) {
        $scope.data = data;
      }).error(function (error) {
        $scope.error = error;
      });
    };
    $scope.updateStatus();


    $scope.interval = $interval(function() {
      $scope.updateStatus();
      console.log("UpdateClientStatus");
    }, 5000);

    $scope.showAdd = function() {
      $scope.add.error = null;
      $scope.addActive = 1;
    }

    $scope.hideAdd = function() {
      $scope.addActive = 0;
    }

    $scope.isAddActive = function() {
      return $scope.addActive == 1;
    }

    $scope.wasAddSuccess = function() {
      return angular.isUndefined($scope.add.error) || $scope.add.error == null;
    }

    $scope.addSubmit = function() {
      $http.put("/client?id=" + $scope.add.id +
                        "&protocol=" + $scope.add.protocol +
                        "&contentmanager=" + $scope.add.contentmanager +
                        "&host=" + $scope.add.host +
                        "&port=" + $scope.add.port +
                        "&min=" + $scope.add.min +
                        "&max=" + $scope.add.max +
                        "&count=" + $scope.add.count
                      ).success(function (data) {
                        if (data.result == "Ok") {
                          $scope.add.error = null;
                          $scope.hideAdd();
                          $route.reload();
                        } else {
                          $scope.add.error = "Add failed";
                        }
                      }).error(function (error) {
                        console.log("Add failed: " + error);
                        $scope.add.error = error;
                      });
    }
  });
