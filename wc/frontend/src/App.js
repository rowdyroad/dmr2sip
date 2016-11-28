import React, { Component } from 'react';
import { connect } from 'react-redux'
import * as Actions from './actions'
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import getMuiTheme from 'material-ui/styles/getMuiTheme';

const theme = getMuiTheme({
  palette: {
    primary1Color:'#36404a'
  }

})
class App extends Component {
  componentDidMount = () => {
    window.addEventListener('resize',  this.onResize);
  }

  onResize = () => {
    this.props.actions.resize(document.documentElement.clientWidth, document.documentElement.clientHeight);
  }

  componentWillUnmount = () => {
      window.removeEventListener('resize', this.onResize);
  }

  render = () => {
    return (<MuiThemeProvider muiTheme={theme}>{this.props.children}</MuiThemeProvider>)
  }
}

let mapDispatch = (dispatch) => {
  return {
    actions: {
      resize: (width, height) => {
        dispatch(Actions.ViewportResize(width, height))
      },
      loginSuccess: (user) => {
        dispatch(Actions.LoginSuccess(user))
      },
    }
  }
}

export default connect(state => { return {state: state}; }, mapDispatch)(App)
