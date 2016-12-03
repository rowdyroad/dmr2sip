import React from 'react'
import {Link} from 'react-router'
import {default as ReactModal} from 'react-modal'
import { Field } from 'redux-form/immutable'

export const RFInput = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<Field
				className="form-control"
				component="input"
				{...props}
				/>
	</div>
)

export const RFSelect = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<Field
				className="form-control"
				component="select"
				{...props}
				>
		{props.children}
		</Field>
	</div>
)



export const Modal = (props) =>(
	<ReactModal
          isOpen={props.isOpen}
          shouldCloseOnOverlayClick={false}
          style={{overlay: { zIndex:10000}, content:{left:'25%', top:'25%', right:'25%', bottom:'auto'}}}
        >
        <Row>
        	<Col11>
        		<h1>{props.title}</h1>
        	</Col11>
        	<Col1>
        		{props.onClose ? <Button color="white" icon="glyphicon glyphicon-remove" onClick={props.onClose}/> : null}
        	</Col1>
        </Row>

          {props.children}
    </ReactModal>
)
export const Input = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<input
				className="form-control"
				disabled={props.disabled ? true : false}
				type={props.type ? props.type : 'text'}
				name={props.name}
				value={props.value}
				placeholder={props.placeholder}
				onChange={props.onChange}/>
	</div>
)

export const Select = (props) => (
	<div className="form-group">
		{ props.label ? <label className="control-label">{props.label}</label> : null}
		<select
				className="form-control"
				disabled={props.disabled ? true : false}
				value={props.value}
				name={props.name}
				placeholder={props.placeholder}
				onChange={props.onChange}>
			{props.children}
		</select>
	</div>
)

export const Box = (props) => (
	<div {...props} className={"card-box " + props.className || ""}>
		{props.children}
	</div>
)

export const Property = (props) => (
	<div>
		<label>{props.label}</label>
		<div>{props.children}</div>
	</div>
)

export const Button = (props) => {

	let classes = ["btn", "waves-effect", "waves-light", "btn-" + (props.color ? props.color : "default")];

	if (props.icon) {
		classes.push("btn-icon");
	}

	let button = (
					<button
							type={props.type ? props.type : 'button'}
							className={classes.join(' ')}
							style={props.style}
							disabled={props.disabled}
							onClick={props.onClick}>
							{props.icon ? <i className={props.icon}></i> : null}
							{props.icon && props.label ? <span>&nbsp;</span> : null}
							{props.label ? <span>{props.label}</span> : null}
					</button>
				)
	return props.to ? (<Link to={props.to}>{button}</Link>) : button;
}


export const Area = (props) => (
	<div>{props.children}</div>
)

export const Row = (props) => (
	<div {...props} className={"row " + (props.className || "")}>
		{props.children}
	</div>
)

export const ProgressBar = (props) => (
	<div className="progress">
	  <div className={"progress-bar progress-bar-" + (props.type || "info")}
	  		role="progressbar"
	  		aria-valuenow={props.progress}
	  		aria-valuemin={props.min || 0}
	  		aria-valuemax={props.max || 100}
	  		style={{width: props.progress +'%'}}>
	    {props.progress}%
	  </div>
	</div>
)


export const Grid = (props) => (
	<Row>
		{(props.items || []).map((item,i) => {
			return <Col
							width={12 / (props.cols || 4)}
							key={i}
							style={{...props.style || {}, padding:'0.5em'}}>
								<Box>
									{React.createElement(props.component, item)}
								</Box>
					</Col>;
		})}
	</Row>
)


export const Col = (props) => (
	<div className={"col-md-" + props.width} {...props}>
		{props.children}
	</div>
)
export const Col1 = (props) => ( <div className="col-md-1" {...props}> {props.children} </div>)
export const Col2 = (props) => ( <div className="col-md-2" {...props}> {props.children} </div>)
export const Col3 = (props) => ( <div className="col-md-3" {...props}> {props.children} </div>)
export const Col4 = (props) => ( <div className="col-md-4" {...props}> {props.children} </div>)
export const Col5 = (props) => ( <div className="col-md-5" {...props}> {props.children} </div>)
export const Col6 = (props) => ( <div className="col-md-6" {...props}> {props.children} </div>)
export const Col7 = (props) => ( <div className="col-md-7" {...props}> {props.children} </div>)
export const Col8 = (props) => ( <div className="col-md-8" {...props}> {props.children} </div>)
export const Col9 = (props) => ( <div className="col-md-9" {...props}> {props.children} </div>)
export const Col10 = (props) => ( <div className="col-md-10" {...props}> {props.children} </div>)
export const Col11 = (props) => ( <div className="col-md-11" {...props}> {props.children} </div>)
export const Col12 = (props) => ( <div className="col-md-12" {...props}> {props.children} </div>)



