import React, { Component } from 'react'
import {Link} from 'react-router'
import { connect } from 'react-redux'
import logo from '../assets/images/logo-w.png'
import * as Actions from '../actions'
import '../assets'
import Request from '../utils/Request'
import Breadcrumbs from 'react-breadcrumbs'

class MainMenu extends Component
{
    componentWillMount = () => {
        var menu = {};
        this.props.routes.forEach((route) => {
            let section = route.link.section;
            if (!menu[section]) {
                menu[section] = [];
            }
            menu[section].push(route);
        });
        this.setState({menu: menu});
    }

    render = () => {
        let menu = [];
        Object.keys(this.state.menu).forEach((name) => {
            menu.push((<MainMenuSection key={name} name={name}></MainMenuSection>));
            this.state.menu[name].forEach((route) => {
                menu.push((<MainMenuItem key={route.path} {...route}></MainMenuItem>))
            });
        })
        return (<div id={this.props.id}>
                        <ul>
                           {menu}
                        </ul>
                        <div className="clearfix"></div>
                    </div>)
    }
}


class MainMenuSection extends Component
{
    render = () => {
        return (<li className="text-muted menu-title">{this.props.name}</li>)
    }
}

class MainMenuItem extends Component
{
    render = () => {
       return (
                <li>
                    <Link to={this.props.path} className="waves-effect">
                        <i className={this.props.link.icon}></i>
                        <span>{this.props.link.title}</span>
                    </Link>
                </li>
            )
    }
}


class MainLayout extends Component
{
    static contextTypes = {
      router: React.PropTypes.object.isRequired
    }

    state = {
        enlarged:false,
        profile_open:false
    }

    toogleMenu = () => {
        this.setState({enlarged: !this.state.enlarged});
    }

    toggleProfile = () => {
        this.setState({profile_open: !this.state.profile_open});
    }

    gotoFullscreen = () => {
        document.getElementById('wrapper').requestFullScreen();
    }

    logout = () => {
        Request.post("/api/user/logout").then(() => {
            this.props.actions.logout();
        })
    }

    shouldComponentUpdate = (nextProps, nextState) => {
        if (!nextProps.state.getIn(['login','success'])) {
            this.context.router.replace("/login");
            return false;
        }
        return true;
    }

	render = () => {
		let isMobile = this.props.state.getIn(['viewport','width']) < 480;
		return (
        <div id="wrapper" className={this.state.enlarged || isMobile ? 'enlarged' : ''} >
            <div className="topbar">
                <div className="topbar-left">
                    <div className="text-center">
                        <Link to="/" className="logo"><img src={logo} className="logo" alt="logo" /> {this.state.enlarged || isMobile ? null : 'DMR2SIP'}</Link>
                    </div>
                </div>
                <div className="navbar navbar-default" role="navigation">
                    <div className="container">
                        <div className="">
                            <div className="pull-left">
                                <button onClick={this.toogleMenu} className="button-menu-mobile open-left waves-effect waves-light">
                                    <i className="md md-menu"></i>
                                </button>
                                <span className="clearfix"></span>
                            </div>

                            <ul className="nav navbar-nav navbar-right pull-right">
                                <li className={"dropdown top-menu-item-xs " + (this.state.profile_open ? 'open' : '')}>
                                    <button onClick={this.toggleProfile}
                                        className="button-menu-mobile open-left waves-effect waves-light"
                                        data-toggle="dropdown"
                                        aria-expanded="true">
                                            <i className="fa fa-user"></i> <span className="profile-name">{this.props.state.getIn(['login','response','name'])}</span>
                                        </button>
                                    <ul className="dropdown-menu">
                                        <li><a href="" onClick={this.logout}><i className="ti-power-off m-r-10 text-danger"></i> Logout</a></li>
                                    </ul>
                                </li>
                            </ul>
                        </div>
                    </div>
                </div>
            </div>
            <div className="left side-menu">
                <div className="sidebar-inner slimscrollleft" style={{height: this.props.state.getIn(['viewport','height']) + 'px'}}>
                    <MainMenu id="sidebar-menu" routes={this.props.route.childRoutes}></MainMenu>
                    <div className="clearfix"></div>
                </div>
            </div>
            <div className="content-page">
                <div className="content">
                  <div className="container">
                    <div className="row">
                        <div className="col-sm-12">
                            <Breadcrumbs  displayMissing={false} routes={this.props.routes} params={this.props.params} />
                        </div>
                    </div>
                    <div className="row">
                        <div className="col-md-12">
                            <div style={{padding:'1em'}}></div>
                            {this.props.children}
                        </div>
                    </div>
                  </div>
                </div>
                <footer className="footer text-right">
                    © 2016. All rights reserved.
                </footer>
            </div>
        </div>
        );
    }
}


let mapDispatch = (dispatch) => {
  return {
    actions: {
      logout: () => {
        dispatch(Actions.ObjectRemove('login'))
      }
    }
  }
}

export default connect(state => { return {state: state.main} }, mapDispatch)(MainLayout)