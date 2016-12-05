import React from 'react'
import {createStore, applyMiddleware,combineReducers} from 'redux'
import {Provider} from 'react-redux'
import thunk from 'redux-thunk'
import {render} from 'react-dom'
import {reducer} from './reducers'
import { Router, Route,  browserHistory, IndexRoute, IndexRedirect } from 'react-router'
import App from './App'
import NotFound from './pages/NotFound'
import LoginForm from './pages/Login'
import * as Actions from './actions'
import MainLayout from './components/MainLayout'
import Request from './utils/Request'
import { reducer as formReducer } from 'redux-form/immutable'
import {ItemFetcher, ListFetcher, Grid,Form, Delete, DeleteForm, Composer, Creator} from './components/entry'

import * as Point from './components/models/point'
import * as Rout from './components/models/route'
import * as Event from './components/models/event'
import * as User from './components/models/user'

const store = createStore(combineReducers({main:reducer, form:formReducer}), window.devToolsExtension(), applyMiddleware(thunk));
const AuthConfig = {
  private: function(nextState, replace) {
      if (!store.getState().main.getIn(['login','success'])) {
        store.dispatch(Actions.LoginRedirect(nextState.location));
        replace('/login');
      }
  },
  publicOnly: function(nextState, replace) {
     if (store.getState().main.getIn(['login','success'])) {
        replace("/");
     }
  }
}


const startApp = () => {
  render(
  <Provider store={store}>
    <Router history={browserHistory}>
      <Route path="/" component={App} name="Home">
          <Route component={MainLayout} onEnter={AuthConfig.private}>
           <IndexRedirect to="/events"/>
           <Route path="/events" name="Events"
                                  component={ListFetcher({ scope:'events', apiUrl:'/api/events', pkAttribute:'event_id' })}
                                  link={{icon:'ti-direction-alt', title:'Events', 'section':'Observation'}}>
              <IndexRoute component={Creator(Event.List, 'events', {component:Event.Preview})}/>
            </Route>

            <Route path="/points" name="Points"
                                  component={ListFetcher({ scope:'points', apiUrl:'/api/points', pkAttribute:'point_id' })}
                                  link={{icon:'ti-desktop',title:'Points', 'section':'Integration'}}>

              <IndexRoute component={Creator(Grid, 'points', {cols:4, component:Point.Preview, newUrl:'/points/new'})}/>

              <Route path="new" name="New" component={Form(Point.Form, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points/:point_id', closeRedirect:'/points'})}/>

              <Route path=":point_id" staticName={true} component={ItemFetcher({ scope: 'point', apiUrl:'/api/points', pkAttribute:'point_id', listUrl:'/points', listScope:'points', autoFetch:true})}>
                <IndexRoute component={Point.View} />
                <Route path="update" name="Update" component={Form(Point.Form, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points/:point_id',  closeRedirect:'/points/:point_id'})}/>
                <Route path="delete" name="Delete" component={Delete(DeleteForm, {scope:'point', pkAttribute:'point_id', listScope:'points', apiUrl:'/api/points', successRedirect:'/points',  closeRedirect:'/points/:point_id'})}/>
              </Route>
            </Route>




            <Route path="/routes" name="Routes"
                                  component={Composer([ListFetcher({ scope:'routes', apiUrl:'/api/routes', pkAttribute:'route_id' }),
                                                       ListFetcher({ scope:'points', map:'points', apiUrl:'/api/points', pkAttribute:'point_id'})])}
                                  link={{icon:'ti-control-shuffle',title:'Routes', 'section':'Integration'}}>

              <IndexRoute component={Creator(Grid, 'routes', {cols:4, component:Rout.Preview, newUrl:'/routes/new'})}/>

              <Route path="new" name="New" component={Form(Rout.Form, {scope:'route', pkAttribute:'route_id', listScope:'routes', apiUrl:'/api/routes', successRedirect:'/routes/:route_id', closeRedirect:'/routes'})}/>

              <Route path=":route_id" staticName={true} component={ItemFetcher({ scope: 'route', apiUrl:'/api/routes', pkAttribute:'route_id', listUrl:'/routes', listScope:'routes', autoFetch:true})}>
                <IndexRoute component={Rout.View} />
                <Route path="update" name="Update" component={Form(Rout.Form, {scope:'route', pkAttribute:'route_id', listScope:'routes', apiUrl:'/api/routes', successRedirect:'/routes/:route_id',  closeRedirect:'/routes/:route_id'})}/>
                <Route path="delete" name="Delete" component={Delete(DeleteForm, {scope:'route', pkAttribute:'route_id', listScope:'routes', apiUrl:'/api/routes', successRedirect:'/routes',  closeRedirect:'/routes/:route_id'})}/>
              </Route>
            </Route>


            <Route path="/users" name="Users"
                                 component={ListFetcher({ scope:'Users', apiUrl:'/api/users', pkAttribute:'user_id' })}
                                 link={{icon:'ti-user',title:'Users', 'section':'Managment'}}>
              <IndexRoute component={Creator(Grid, 'users', {cols:4, component:User.Preview})}/>
              <Route path=":user_id" staticName={true} component={ItemFetcher({ scope: 'user', apiUrl:'/api/users', pkAttribute:'user_id', listUrl:'/users', listScope:'users', autoFetch:true})}>
                <IndexRoute component={User.View} />
                <Route path="update" name="Update" component={Form(User.Form, {scope:'user', pkAttribute:'user_id', listScope:'users', apiUrl:'/api/users', successRedirect:'/users',  closeRedirect:'/users'})}/>
              </Route>
            </Route>
            <Route path="/service" name="Service" link={{icon:'ti-settings',title:'Service', 'section':'Settings'}}/>
            <Route path="/mixer" name="Mixer"  link={{icon:'ti-volume',title:'Mixer', 'section':'Settings'}}/>
          </Route>
          <Route path="/login" component={Form(LoginForm, {scope:'login', pkAttribute:'user_id', apiUrl:'/api/user/login', successRedirect:'/'})} onEnter={AuthConfig.publicOnly}/>
      </Route>
      <Route path="*" component={NotFound}/>
    </Router>
  </Provider>,
    document.getElementById('root')
  );
}

Actions.Fetch('login', '/api/user/check')(store.dispatch, store.getState().main).then((e)=> {
  startApp();
});
