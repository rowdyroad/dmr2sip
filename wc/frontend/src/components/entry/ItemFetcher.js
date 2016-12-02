import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions';
import {Router, browserHistory, Link} from 'react-router'

class Item extends Component
{
    componentWillMount = () =>{
        this.props.actions.fetch(this.props.params[this.props.pkAttribute])
    }

    componentWillUpdate = (next) => {
      if (next.data && next.route.staticName) {
        next.route.name = next.data.name;
        next.actions.setState({title: next.data.name});
      }

      if (next.context && next.context.error) {
        this.props.actions.error(next.context.error);
      }

      if (!next.context && this.props.context) {
        this.props.actions.error(404);
      }
    }

    render = () => {
        if (!this.props.data) {
            return null; //todo loader
        }
        return (
                    <div>
                        {React.Children.map(this.props.children, (child)=> {
                          return React.cloneElement(child, {...this.props, ...child.props});
                        })}
                    </div>
              )
    }
}

Item.propTypes = {
    pkAttribute:React.PropTypes.string.isRequired,
    actions: React.PropTypes.shape({
      fetch:React.PropTypes.func.isRequired,
      setState: React.PropTypes.func.isRequired
    }),
    children: React.PropTypes.any.isRequired
}


export default (props) => {
  let mapState = (state) => {
      return {
                  ...props,
                  data: state.main.getIn([props.scope,'success']) ? state.main.getIn([props.scope,'response']).toJS() : null,
                  context: state.main.has(props.scope) ? state.main.get(props.scope).toJS() : null,
              }
  }

  let mapDispatch = (dispatch) => {

    return {
      actions: {
        fetch: (id) => {
          dispatch(Actions.Fetch(props.scope, props.apiUrl + "/" + id, {
            onSuccess:(data) => {
              return props.listScope ? Actions.ListItemUpdate([props.listScope,"response","data"], data, props.pkAttribute) : false
            }
          }));
        },
        delete:(data) => {
          dispatch(Actions.Delete(props.scope, props.apiUrl + "/" + data[props.pkAttribute], {}, {
              onSuccess:props.listScope ? Actions.ListItemRemove([props.listScope,"response","data"], props.pkAttribute, data[props.pkAttribute]) : false
          }));
        },
        setState:(state) => {
          dispatch(Actions.ObjectMerge(props.scope, state))
        },
        close: () => {
          browserHistory.replace(props.listUrl || "/");
        },
        notFound: () => {
          browserHistory.replace(props.listUrl || "/");
        },
        error: (error) => {
          browserHistory.replace(props.notFoundUrl || "/");
        }
      }
    }
  }

 return connect(mapState, mapDispatch)(Item)
}