import React, { Component, PropTypes } from 'react';
import {indigo500} from 'material-ui/styles/colors';
import Request from '../utils/Request';

class LoginForm extends Component {
  state = {
    login : '',
    password:''
  }

  submitForm = (e) => {
    e.preventDefault();
    this.props.actions.login();
    Request.post(
        '/api/user/login', {
          username: this.state.login,
          password: this.state.password
        })
        .then(this.props.actions.loginSuccess)
        .catch(this.props.actions.loginFailure);
  }

  getState = () => { return this.props.login.state; }

  setLogin = (e) => { this.setState({login: e.target.value}); }

  setPassword = (e) => { this.setState({password: e.target.value}); }

  render()
  {
    let loading =  this.props.login.state == 'requested';

    return (
          <div className="form">
          {this.getState() == 'error' ? (<div style={{color:'red', textAlign:'center'}}>Incorrect password</div>) : null}
            <form onSubmit={this.submitForm}>
              <div className="form-group">
                <label for="login">Login</label>
                <input id="login"
                      type="text"
                      placeholder="Login"
                      autoComplete="off"
                      required="true"
                      className="form-control"
                      value={this.state.login}
                      onChange={this.setLogin}
                      />
              </div>
              <div className="form-group">
                <label for="password">Password</label>
                <input id="password"
                      type="password"
                      placeholder="Password"
                      autoComplete="off"
                      required="true"
                      className="form-control"
                      value={this.state.password}
                      onChange={this.setPassword}
                      />
              </div>
              <div style={{ textAlign:'center', marginTop:'1em'}}>
                <button className="btn btn-primary" disabled={loading} type="submit">Login</button>
              </div>
            </form>
          </div>
    )
  }
}

export default LoginForm;